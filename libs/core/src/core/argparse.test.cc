#include "core/argparse.h"

#include "core/enumtostring.h"
#include "core/fourway.h"
#include "tests/utils.h"
#include "tests/stringeq.h"
#include "tests/vectorequals.h"

using namespace euphoria::core;
using namespace euphoria::core::argparse;

using namespace euphoria::tests;


namespace
{
    name_and_arguments
    make_arguments(const std::vector<std::string>& args)
    {
        return name_and_arguments{ "app", args };
    }


    struct message
    {
        bool error;
        std::string text;

        message(bool e, const std::string& t)
            : error(e)
            , text(t)
        {
        }
    };


    message
    create_info(const std::string& text)
    {
        return {false, text};
    }


    message
    create_error(const std::string& text)
    {
        return {true, text};
    }


    template<typename T>
    std::ostream&
    operator<<(std::ostream& o, const std::vector<T>& m)
    {
        o << "[";
        bool first = true;
        for(const auto& t: m)
        {
            if(first) {first = false;}
            else { o << ", "; }
            o << t;
        }
        o << "]";
        return o;
    }


    std::ostream&
    operator<<(std::ostream& o, const message& m)
    {
        o << (m.error?"ERR":"INF");
        o << " " << m.text;
        return o;
    }


    struct test_printer : public printer
    {
        std::vector<message> messages;

        void
        print_error(const std::string& line) override
        {
            messages.emplace_back(true, line);
        }

        void
        print_info(const std::string& line) override
        {
            messages.emplace_back(false, line);
        }
    };


    false_string
    check
    (
        const std::vector<std::string>& lhs,
        const std::vector<std::string>& rhs
    )
    {
        return euphoria::tests::vector_is_equal
        (
            lhs,
            rhs,
            [](const std::string &m) -> std::string
            {
                return m;
            },
            [](const std::string &lhs, const std::string &rhs) -> false_string
            {
                return string_is_equal(lhs, rhs);
            }
        );
    }

    false_string
    check
    (
        const std::vector<message>& lhs,
        const std::vector<message>& rhs
    )
    {
        return euphoria::tests::vector_is_equal
        (
            lhs,
            rhs,
            [](const message &m) -> std::string
            {
                return string_builder() << m;
            },
            [](const message &lhs, const message &rhs) -> false_string
            {
                const auto str = string_is_equal(lhs.text, rhs.text);
                if (str == false)
                { return str; }
                if (lhs.error == rhs.error)
                { return false_string::create_true(); }
                return false_string::create_false
                (
                    string_builder() << "error diff: "
                                     << lhs.error << " vs "
                                     << rhs.error
                );
            }
        );
    }




    enum class animal
    {
        cat, dog, bird, none
    };


    std::ostream&
    operator<<(std::ostream& o, const animal& m)
    {
        o << euphoria::core::enum_to_string(m);
        return o;
    }
}


TEST_CASE("argparse", "[argparse]")
{
    auto parser = argparse::parser{};
    auto output = std::make_shared<test_printer>();
    parser.printer = output;


    SECTION("default")
    {
        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit")
            }));
        }

        SECTION("positional error")
        {
            const auto res = parser.parse(make_arguments({"dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_error("'dog' was unexpected")
            }));
        }

        SECTION("optional error")
        {
            const auto res = parser.parse(make_arguments({"--cat" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_error("unknown argument: '--cat', did you mean '--help'?")
            }));
        }
    }

    SECTION("void function")
    {
        std::string var = "default";
        parser.add_void_function("-f", [&]
        {
            var = "called";
        }).set_help("call function");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(var == "default");
        }

        SECTION("function called")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::ok);
            CHECK(var == "called");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f          call function")
            }));
        }

        SECTION("call function but die anyway")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(var == "called");
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f]"),
                create_error("'dog' was unexpected")
            }));
        }
    }

    SECTION("optional int")
    {
        int value = 0;
        parser.add("-f", &value).set_help("a int");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == 0);
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(make_arguments({"-f", "42" }));
            CHECK(res == argparse::ok);
            CHECK(value == 42);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        a int (default: 0)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }

        SECTION("string is not a int")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("'dog' is not accepted for '-f'")
            }));
        }
    }

    SECTION("optional string")
    {
        std::string value = "default";
        parser.add("-f", &value).set_help("some string");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == "default");
        }

        SECTION("parse dog")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(value == "dog");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        some string (default: default)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }
    }


    SECTION("optional enum")
    {
        animal value = animal::dog;
        parser.add("-f", &value).set_help("some animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == animal::dog);
        }

        SECTION("parse cat")
        {
            const auto res = parser.parse(make_arguments({"-f", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == animal::cat);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        some animal (default: dog)"),
                create_info("              can be either 'cat', 'dog', 'bird' or 'none'")
            }));
        }

        SECTION("parse missing")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }

        SECTION("parse mouse")
        {
            const auto res = parser.parse(make_arguments({"-f", "mouse" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("'mouse' is not accepted for '-f', did you mean 'none', 'dog', 'bird' or 'cat'?")
            }));
        }
    }

    SECTION("multi optional names")
    {
        animal value = animal::dog;
        parser.add("-a, --animal", &value).set_help("set the animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == animal::dog);
        }

        SECTION("short name")
        {
            const auto res = parser.parse(make_arguments({"-a", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == animal::cat);
        }

        SECTION("long name")
        {
            const auto res = parser.parse(make_arguments({"--animal", "bird" }));
            CHECK(res == argparse::ok);
            CHECK(value == animal::bird);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-a A]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help      show this help message and exit"),
                create_info("  -a, --animal A  set the animal (default: dog)"),
                create_info("                  can be either 'cat', 'dog', 'bird' or 'none'")
            }));
        }

        SECTION("--animal cookie error")
        {
            const auto res = parser.parse(make_arguments({"--animal", "cookie" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] [-a A]"),
                    create_error("'cookie' is not accepted for '--animal', did you mean 'none', 'dog', 'cat' or 'bird'?")
            }));
        }
        SECTION("-a cake error")
        {
            const auto res = parser.parse(make_arguments({"-a", "cookie" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] [-a A]"),
                    create_error("'cookie' is not accepted for '-a', did you mean 'none', 'dog', 'cat' or 'bird'?")
            }));
        }
    }

    SECTION("positional string")
    {
        std::string value = "default";
        parser.add("f", &value);

        // todo(Gustav): add int parse error test

        SECTION("positional missing = error")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::error);
            CHECK(value == "default");
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] F"),
                    create_error("Positional F was not specified.")
            }));
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(make_arguments({"dog" }));
            CHECK(res == argparse::ok);
            CHECK(value == "dog");
        }
    }

    SECTION("greedy")
    {
        int a = 42;
        int b = 42;
        std::vector<std::string> files;

        parser.add("-a, --alpha", &a).set_allow_before_positionals().set_help("set a");
        parser.add("-b, --beta", &b).set_allow_before_positionals().set_help("set b");
        parser.add_vector("files", &files).set_help("the files");

        SECTION("parse single")
        {
            const auto res = parser.parse(make_arguments({"dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(check(files,
                        {
                "dog"
            }));
        }

        SECTION("parse many")
        {
            const auto res = parser.parse(make_arguments({"dog", "cat"}));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(check(files,
                        {
                "dog", "cat"
            }));
        }

        SECTION("set single short argument")
        {
            const auto res = parser.parse(make_arguments({"-a", "5", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 42);
            CHECK(check(files,
                        {
                "dog"
            }));
        }

        SECTION("set single long argument")
        {
            const auto res = parser.parse(make_arguments({"--alpha", "7", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 7);
            CHECK(b == 42);
            CHECK(check(files,
                        {
                "dog"
            }));
        }

        SECTION("set many argument")
        {
            const auto res = parser.parse(make_arguments({"-a", "5", "-b", "3", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 3);
            CHECK(check(files,
                        {
                "dog"
            }));
        }
    }

    SECTION("subparser")
    {
        std::string a = "default";
        std::string b = "default";

        auto sub = parser.add_sub_parsers();

        sub->add("a", "do awesome stuff", [&](sub_parser* parser)
        {
            std::string a_value = "dog";
            parser->add("-s", &a_value);

            return parser->on_complete([&]
            {
                a = a_value;
                return argparse::ok;
            });
        });

        sub->add("b", "do boring stuff", [&](sub_parser*)
        {
            b = "bird";
            return argparse::ok;
        });

        SECTION("empty subparser = error")
        {
            const auto res = parser.parse(make_arguments({}));
            INFO(output->messages);
            CHECK(res == argparse::error);
            CHECK(a == "default");
            CHECK(b == "default");
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("no subparser specified")
            }));
        }

        SECTION("call a")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "a"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "dog");
            CHECK(b == "default");
        }

        SECTION("call a with arg")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "a", "-s", "cat"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "cat");
            CHECK(b == "default");
        }

        SECTION("call b")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "b"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "default");
            CHECK(b == "bird");
        }

        SECTION("subcommand help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  a           do awesome stuff"),
                    create_info("  b           do boring stuff"),
            }));
        }

        SECTION("bad subcommand")
        {
            const auto res = parser.parse(make_arguments({"cat" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'cat', did you mean 'a' or 'b'?")
            }));
        }
        SECTION("invalid optional for root")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("unknown argument: '-f', did you mean '-h'?")
            }));
        }
    }

    SECTION("non greedy subparser/script like")
    {
        auto sub = parser.add_sub_parsers();
        parser.parser_style = sub_parser_style::fallback;
        std::string data;
        sub->add("add", "add something", [&](sub_parser* sub)
        {
            std::string what;
            sub->add("what", &what);
            return sub->on_complete([&]
            {
                data += what;
                return argparse::ok;
            });
        });
        sub->add("double", "double the content", [&](sub_parser* sub)
        {
            return sub->on_complete([&]
            {
                data += data;
                return argparse::ok;
            });
        });

        SECTION("once")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "add", "dog"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "add", "cat",
                    "double"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "catcat");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  add         add something"),
                    create_info("  double      double the content"),
            }));
        }

        SECTION("add and error")
        {
            const auto res = parser.parse(make_arguments({"add", "dog", "dog" }));
            CHECK(res == argparse::error);
            CHECK(data == "dog");
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'dog', did you mean 'add' or 'double'?")
            }));
        }
    }

    SECTION("non root root")
    {
        auto sub = parser.add_sub_parsers();
        std::string data;
        sub->add("pretty", "be kind", [&](sub_parser* pretty)
        {
            pretty->add_sub_parsers()->add("please", [&](sub_parser* please)
            {
                please->parser_style = sub_parser_style::fallback;
                auto sub = please->add_sub_parsers();
                sub->add("add", [&](sub_parser* sub)
                {
                    std::string what;
                    sub->add("what", &what);
                    return sub->on_complete([&]
                    {
                        data += what;
                        return argparse::ok;
                    });
                });
                sub->add("double", [&](sub_parser* sub)
                {
                    return sub->on_complete([&]
                    {
                        data += data;
                        return argparse::ok;
                    });
                });
                return please->on_complete([]{return argparse::ok;});
            });
            return pretty->on_complete([]{return argparse::ok;});
        });

        SECTION("once")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "pretty", "please", "add", "dog"
                })
            );
            CHECK(res == argparse::ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto arguments =
                make_arguments
                ({
                    "pretty", "please", "add", "cat",
                    "please", "double"
                });
            const auto res = parser.parse
            (
                arguments
            );
            INFO(output->messages);
            INFO(arguments);
            REQUIRE(data == "catcat");
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  pretty      be kind"),
            }));
        }

        SECTION("error but no add")
        {
            const auto res = parser.parse(make_arguments({"pretty", "dog" }));
            CHECK(res == argparse::error);
            REQUIRE(data.empty());
            CHECK(check(output->messages,
                        {
                    create_info("usage: app pretty [-h] <command> [<args>]"),
                    create_error("Invalid command 'dog', did you mean 'please'?")
            }));
        }
    }
}


TEST_CASE("argparse_error", "[argparse]")
{
    auto parser = argparse::parser{};
    auto output = std::make_shared<test_printer>();
    parser.printer = output;

    SECTION("default")
    {
        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"dog"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"-o"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"--make-cool"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("fourway test int")
    {
        using FF = fourway<int>;
        auto ff = FF{0};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(make_arguments({"4"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{4});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(make_arguments({"4/2"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(2, 4));
        }
        SECTION("all values")
        {
            const auto res = parser.parse(make_arguments({"1/2/3/4"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(4, 2, 1, 3));
        }
    }

    SECTION("fourway test enum")
    {
        using FF = fourway<animal>;
        auto ff = FF{animal::none};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(make_arguments({"cat"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{animal::cat});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(make_arguments({"cat/none"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(animal::none, animal::cat));
        }
    }

    SECTION("sub parser greedy")
    {
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = sub_parser_style::greedy;
        sub->add("a", [&](sub_parser* parser)
        {
            return parser->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("sub parser fallback")
    {
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = sub_parser_style::fallback;
        sub->add("a", [&](sub_parser* parser)
        {
            return parser->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'dog', did you mean 'a'?")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'cat', did you mean 'a'?")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app a [-h]"),
                    create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }
}

