#ifndef EUPHORIA_DUK_BINDOBJECT_H
#define EUPHORIA_DUK_BINDOBJECT_H

#include <string>
#include <vector>

#include "duk/bind.h"

namespace euphoria::duk
{
    struct ObjectBinder
    {
    public:
        ObjectBinder&
        AddFunction(
                const std::string&               name,
                const std::shared_ptr<Function>& bind);

        struct MemberFunction
        {
            std::string               name;
            std::shared_ptr<Function> function;
        };

        std::vector<MemberFunction> functions;
    };

    ObjectBinder
    BindObject();
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_BINDOBJECT_H