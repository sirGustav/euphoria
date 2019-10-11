#ifndef EUPHORIA_PROPERTYTREE_H
#define EUPHORIA_PROPERTYTREE_H

#include <string>
#include <map>
#include <memory>

#include "core/vec3.h"

namespace euphoria::core
{
    enum class ValueType
    {
        Int,
        Float,
        Vec3f,
        Point3f,
        Struct
    };

    struct Value
    {
        explicit Value(ValueType vt) : type(vt) {}

        virtual ~Value() = default;

        const ValueType type;
    };

    struct ValueInt : public Value
    {
        explicit ValueInt(int i);
        int value;

        [[nodiscard]] static int&
        Cast(Value* value);
    };

    struct ValueFloat : public Value
    {
        explicit ValueFloat(float f);
        float value;

        [[nodiscard]] static float&
        Cast(Value* value);
    };

    struct ValueVec3f : public Value
    {
        explicit ValueVec3f(const vec3f& v);
        vec3f value;

        [[nodiscard]] static vec3f&
        Cast(Value* value);
    };

    struct ValuePoint3f : public Value
    {
        explicit ValuePoint3f(const vec3f& v);
        vec3f value;

        [[nodiscard]] static vec3f&
        Cast(Value* value);
    };

    struct PropertyTree : public Value
    {
        PropertyTree();

        void
        Set(const std::string& name, std::shared_ptr<Value> value);

        std::shared_ptr<Value>
        GetOrNull(const std::string& name);

        std::map<std::string, std::shared_ptr<Value>> properties;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_PROPERTYTREE_H
