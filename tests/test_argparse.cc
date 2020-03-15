#include "core/argparse.h"

#include "utils.h"
#include "core/enumtostring.h"


using namespace euphoria::core::argparse;

namespace
{
    Arguments
    MakeArguments(const std::vector<std::string>& args)
    {
        return Arguments{ "path/to/app", args };
    }

    struct Message
    {
        bool error;
        std::string text;

        Message(bool e, const std::string& t)
            : error(e)
            , text(t)
        {
        }
    };


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
    operator<<(std::ostream& o, const Message& m)
    {
        o << (m.error?"ERR":"INF");
        o << " " << m.text;
        return o;
    }

    struct TestPrinter : public Printer
    {
        std::vector<Message> messages;

        void
        PrintError(const std::string& line) override
        {
            messages.emplace_back(true, line);
        }

        void
        PrintInfo(const std::string& line) override
        {
            messages.emplace_back(false, line);
        }
    };

    enum class Animal
    {
        Cat, Dog, Bird
    };


    std::ostream&
    operator<<(std::ostream& o, const Animal& m)
    {
        o << euphoria::core::EnumToString(m);
        return o;
    }
    
}


TEST_CASE("argparse", "[argparse]")
{
    auto parser = Parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;


    SECTION("default")
    {
        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
        }
    }

    SECTION("void function")
    {
        std::string var = "default";
        parser.AddVoidFunction("-f", [&]
        {
            var = "called";
        });

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(var == "default");
        }

        SECTION("function called")
        {
            const auto res = parser.Parse(MakeArguments({ "-f" }));
            CHECK(res == ParseResult::Ok);
            CHECK(var == "called");
        }
    }

    SECTION("optional int")
    {
        int value = 0;
        parser.Add("-f", &value);

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == 0);
        }

        SECTION("parse 42")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "42" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == 42);
        }
    }

    SECTION("optional string")
    {
        std::string value = "default";
        parser.Add("-f", &value);

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "default");
        }

        SECTION("parse 42")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "dog");
        }
    }


    SECTION("optional enum")
    {
        Animal value = Animal::Dog;
        parser.Add("-f", &value);

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("parse cat")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "cat" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Cat);
        }
    }

    SECTION("multi optional names")
    {
        Animal value = Animal::Dog;
        parser.Add("-a, --animal", &value);

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("short name")
        {
            const auto res = parser.Parse(MakeArguments({ "-a", "cat" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Cat);
        }

        SECTION("long name")
        {
            const auto res = parser.Parse(MakeArguments({ "--animal", "bird" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Bird);
        }
    }

    SECTION("positional string")
    {
        std::string value = "default";
        parser.Add("f", &value);

        SECTION("positional missing = error")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Error);
            CHECK(value == "default");
        }

        SECTION("parse 42")
        {
            const auto res = parser.Parse(MakeArguments({ "dog" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "dog");
        }
    }

    SECTION("subparser")
    {
        std::string a = "default";
        std::string b = "default";

        auto sub = parser.AddSubParsers();

        sub->Add("a", [&](SubParser* parser)
        {
            std::string a_value = "dog";
            parser->Add("-s", &a_value);

            return parser->OnComplete([&]
            {
                a = a_value;
                return ParseResult::Ok;
            });
        });

        sub->Add("b", [&](SubParser*)
        {
            b = "bird";
            return ParseResult::Ok;
        });

        SECTION("empty subparser = error")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Error);
            CHECK(a == "default");
            CHECK(b == "default");
        }

        SECTION("call a")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "a"
                })
            );
            CHECK(res == ParseResult::Ok);
            CHECK(a == "dog");
            CHECK(b == "default");
        }

        SECTION("call a with arg")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "a", "-s", "cat"
                })
            );
            CHECK(res == ParseResult::Ok);
            CHECK(a == "cat");
            CHECK(b == "default");
        }

        SECTION("call b")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "b"
                })
            );
            CHECK(res == ParseResult::Ok);
            CHECK(a == "default");
            CHECK(b == "bird");
        }
    }

    SECTION("non greedy subparser/script like")
    {
        auto sub = parser.AddSubParsers();
        std::string data;
        parser.sub_parser_style = SubParserStyle::Fallback;
        sub->Add("add", [&](SubParser* sub)
        {
            std::string what;
            sub->Add("what", &what);
            return sub->OnComplete([&]
            {
                data += what;
                return ParseResult::Ok;
            });
        });
        sub->Add("double", [&](SubParser* sub)
        {
            return sub->OnComplete([&]
            {
                data += data;
                return ParseResult::Ok;
            });
        });

        SECTION("once")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "add", "dog"
                })
            );
            CHECK(res == ParseResult::Ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "add", "cat",
                    "double"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(data == "catcat");
        }
    }
}
