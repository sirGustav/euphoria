#include "core/argparse.h"

#include "core/cint.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"

#include <iostream>
#include <iomanip>


/*

   argparse issues & todos:

   * output looks horrible

   * unhelpful error messages, hint misspelled argument via StringToEnum code

   * positionals eat help argument! -h with positional generates parsing error

   * assert invalid setups and arguments

   * non-greedy sub commands to operate lite ImageMagic "scripting": https://imagemagick.org/script/magick-script.php

   * Narg chainging functions renaming on return value of Add

   * ParseResult should be a struct/handle custom arguments

   * generic OnComplete unit test

*/


namespace euphoria::core::argparse
{
    namespace
    {
        bool
        IsOptionalArgument(const std::string& name)
        {
            return name[0] == '-';
        }

        bool
        IsValidArgumentName(const std::string& str)
        {
            if (str.empty())
            {
                return false;
            }

            constexpr auto VALID_ARGUMENT_NAME_CHARACTERS = "abcdefghijklmnopqrstuwxyz-_";
            const auto first_invalid_character = ToLower(str).find_first_not_of(VALID_ARGUMENT_NAME_CHARACTERS);
            if (first_invalid_character != std::string::npos)
            {
                return false;
            }

            return true;
        }
    }


    FileOutput::FileOutput(const std::string& o) : file(o), single(!(EndsWith(o, "/") || EndsWith(o, "\\")))
    {}


    std::string
    FileOutput::NextFile(bool print)
    {
        if(single)
        {
            return file;
        }
            
        std::ostringstream ss;
        index += 1;
        if(print)
        {
            std::cout << "Generating " << index << "...\n";
        }

        // todo(Gustav): provide option for file extension
        ss << file << std::setfill('0') << std::setw(5) << index << ".png";
        return ss.str();
    }


    void
    FileOutput::CreateDirIfMissing() const
    {
        if(single) return;
        // std::filesystem::create_directories(file);
    }


    int
    ReturnValue(ParseResult pr)
    {
        switch (pr)
        {
        case ParseResult::Ok: return 0;
        case ParseResult::Error: return -1;
        case ParseResult::Quit: return 0;
        default: return -1;
        }
    }


    Arguments::Arguments(const std::string& n, const std::vector<std::string>& a)
        : name(n)
        , arguments(a)
    {
    }


    Arguments
    Arguments::Extract(int argc, char* argv[])
    {
        auto ret = Arguments{argv[0] , {}};
        
        for (int i = 1; i < argc; i += 1)
        {
            ret.arguments.emplace_back(argv[i]);
        }

        return ret;
    }


    ArgumentReader::ArgumentReader(const Arguments& a)
        : arguments(a)
        , next_position(0)
    {
    }


    bool
    ArgumentReader::HasMore() const
    {
        return next_position < Csizet_to_int(arguments.arguments.size());
    }


    std::string
    ArgumentReader::Peek() const
    {
        if (HasMore())
        {
            const auto arg = arguments.arguments[next_position];
            return arg;
        }
        else
        {
            return "";
        }
    }


    std::string
    ArgumentReader::Read()
    {
        const auto arg = Peek();
        next_position += 1;
        return arg;
    }


    Printer::~Printer()
    {
    }


    void
    ConsolePrinter::PrintError(const std::string& line)
    {
        std::cerr << line << "\n";
    }


    void
    ConsolePrinter::PrintInfo(const std::string& line)
    {
        std::cout << line << "\n";
    }


    Name::Name(const char* nn)
        : names(Split(nn, ','))
    {
        for (auto& n : names)
        {
            n = Trim(n);
        }

        ASSERTX(Validate().empty(), Validate());
    }


    std::string
    Name::Validate() const
    {
        if (names.empty()) { return "no names found"; }
        if (IsOptional())
        {
            for (const auto& n : names)
            {
                if (IsOptionalArgument(n) == false)
                {
                    return "arguments are optional but '" + n + "' is not";
                }

                if (IsValidArgumentName(n) == false)
                {
                    return "optional arguments '" + n + "' is not considered valid";
                }
            }
        }
        else
        {
            const auto n = names[0];
            if (IsValidArgumentName(n) == false)
            {
                return "positional arguments '" + n + "' is not considered valid";
            }
        }

        return "";
    }


    bool
    Name::IsOptional() const
    {
        if (names.empty())
        {
            return false;
        }

        if (names.size() > 1)
        {
            return true;
        }

        return IsOptionalArgument(names[0]);
    }


    Argument::~Argument()
    {
    }


    Argument&
    Argument::Help(const std::string& h)
    {
        help = h;
        return *this;
    }


    ArgumentAndName::ArgumentAndName(const Name& n, std::shared_ptr<Argument> a)
        : name(n)
        , argument(a)
    {
    }


    ArgumentNoValue::ArgumentNoValue(Callback cb)
        : callback(cb)
    {
    }


    ParseResult
    ArgumentNoValue::Parse(Runner* runner)
    {
        return callback(runner);
    }


    SingleArgument::SingleArgument(Callback cb)
        : callback(cb)
    {
    }


    ParseResult
    SingleArgument::Parse(Runner* runner)
    {
        if (runner->arguments->HasMore())
        {
            auto res = callback(runner, runner->arguments->Read());
            return res;
        }
        else
        {
            // todo(Gustav): improve eof error message by including argument name
            runner->printer->PrintError("missing argument");
            return ParseResult::Error;
        }
    }


    SubParserContainer::SubParserContainer(SubParserCallback cb)
        : callback(cb)
    {
    }


    Argument&
    ParserBase::AddArgument(const Name& name, std::shared_ptr<Argument> argument)
    {
        if (name.IsOptional())
        {
            for (const auto& n : name.names)
            {
                optional_arguments[n] = argument;
            }
            optional_argument_list.emplace_back(name, argument);
        }
        else
        {
            positional_argument_list.emplace_back(name, argument);
        }
        return *argument;
    }


    Argument&
    ParserBase::AddVoidFunction(const Name& name, std::function<void()> void_function)
    {
        return AddArgument(name, std::make_shared<ArgumentNoValue>([void_function](Runner*) {void_function(); return ParseResult::Ok; }));
    }


    Argument&
    ParserBase::SetTrue(const Name& name, bool* target)
    {
        *target = false;
        return SetConst(name, target, true);
    }


    Argument&
    ParserBase::SetFalse(const Name& name, bool* target)
    {
        *target = true;
        return SetConst(name, target, false);
    }


    void
    ParserBase::OnComplete(CompleteFunction cf)
    {
        on_complete = cf;
    }


    void
    ParserBase::AddSubParser
    (
        const std::string& name,
        const std::string& desc,
        SubParserCallback sub
    )
    {
        auto container = std::make_shared<SubParserContainer>(sub);
        subparsers.Add(name, container);
    }


    void
    ParserBase::AddSubParser
    (
        const std::string& name,
        SubParserCallback sub
    )
    {
        AddSubParser(name, "", sub);
    }


    std::shared_ptr<Argument>
    ParserBase::FindArgument(const std::string& name)
    {
        return optional_arguments[name];
    }


    ParseResult
    ParserBase::ParseArgs(Runner* runner)
    {
        int positional_index = 0;

        auto has_more_positionals = [&]()
        {
            return positional_index < Csizet_to_int(positional_argument_list.size());
        };

        while (runner->arguments->HasMore())
        {
            if (has_more_positionals())
            {
                auto match = positional_argument_list[positional_index];
                auto arg_parse_result = match.argument->Parse(runner);
                if (arg_parse_result != ParseResult::Ok)
                {
                    return arg_parse_result;
                }
                positional_index += 1;
            }
            else
            {
                const auto arg = runner->arguments->Read();
                if(IsOptionalArgument(arg))
                {
                    auto match = FindArgument(arg);
                    if (match == nullptr)
                    {
                        runner->printer->PrintError("invalid argument: " + arg);
                        return ParseResult::Error;
                    }
                    auto arg_parse_result = match->Parse(runner);
                    if (arg_parse_result != ParseResult::Ok)
                    {
                        return arg_parse_result;
                    }
                }
                else
                {
                    // arg doesn't look like a optional argument
                    // hence it must be a sub command
                    // or it's a error
                    if(subparsers.size == 0)
                    {
                        runner->printer->PrintError("no subcomands for " + arg);
                        return ParseResult::Error;
                    }
                    else
                    {
                        auto match = subparsers.Match(arg, 3);
                        if(match.single_match == false)
                        {
                            runner->printer->PrintError("too many matches for " + arg);
                            return ParseResult::Error;
                        }
                        else
                        {
                            auto sub = SubParser{runner};
                            return match.values[0]->callback(&sub);
                        }
                    }
                }
            }
        }

        if (has_more_positionals())
        {
            runner->printer->PrintError("positionals left");
            return ParseResult::Error;
        }

        if(subparsers.size != 0)
        {
            runner->printer->PrintError("no subparser specified");
            return ParseResult::Error;
        }

        if(on_complete.has_value())
        {
            on_complete.value()();
        }

        return ParseResult::Ok;
    }


    SubParser::SubParser(Runner* r)
        : runner(r)
    {
    }


    ParseResult
    SubParser::OnComplete(CompleteFunction com)
    {
        ParserBase::OnComplete(com);
        return ParseArgs(runner);
    }


    Parser::Parser(const std::string& d)
        : description(d)
        , printer(std::make_shared<ConsolePrinter>())
    {
        AddArgument("-h", std::make_shared<ArgumentNoValue>([this](Runner*) {PrintHelp(); return ParseResult::Quit; }))
            .help = "print help";
    }


    void
    Parser::PrintHelp()
    {
        if (description.empty() == false)
        {
            printer->PrintInfo(description);
        }

        // todo(Gustav): use a string table here, add wordwrap to table
        for (auto& a : positional_argument_list)
        {
            const auto names = StringMerger::Comma().Generate(a.name.names);
            printer->PrintInfo(names + " " + a.argument->help);
        }
        for (auto& a : optional_argument_list)
        {
            printer->PrintInfo(a.name.names[0] + " " + a.argument->help);
        }
    }


    ParseResult
    Parser::Parse(const Arguments& args)
    {
        auto reader = ArgumentReader{ args };
        auto runner = Runner{ &reader, printer };
        return ParserBase::ParseArgs(&runner);
    }


    std::optional<int>
    Parser::Parse(int argc, char* argv[])
    {
        const auto args = Arguments::Extract(argc, argv);
        const auto res = Parse(args);
        if (res == ParseResult::Ok)
        {
            return std::nullopt;
        }
        else
        {
            return ReturnValue(res);
        }
    }


    int
    ParseFromMain(Parser* parser, int argc, char* argv[])
    {
        const auto args = Arguments::Extract(argc, argv);
        const auto res = parser->Parse(args);
        return ReturnValue(res);
    }
}
