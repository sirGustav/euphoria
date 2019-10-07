#ifndef CORE_CMDLINE_H
#define CORE_CMDLINE_H

#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace euphoria::core
{
    class CmdLine
    {
    public:
        typedef std::vector<std::string>         Args;
        typedef std::function<void(const Args&)> Callback;

        CmdLine(std::ostream* out);

        void
        Register(const std::string& name, Callback callback);

        void
        Run(const std::string& cmd);

        void
        PrintHelp(const Args& args) const;

    private:
        std::ostream*                   out;
        std::map<std::string, Callback> callbacks;
    };
}  // namespace euphoria::core

#endif  // CORE_CMDLINE_H
