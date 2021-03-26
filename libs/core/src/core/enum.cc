#include "core/enum.h"

#include <iostream>
#include <set>
#include <utility>

#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/vfs_path.h"

#ifdef assert
#undef assert
#endif
#include <cassert>

#include "gaf_enum.h"

namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.enum")


    EnumType::EnumType(std::string name)
        : name_(std::move(name)), isAdding_(true), nextIndex_(1)
    {}


    EnumType::~EnumType()
    {
        ASSERT(!isAdding_);
    }


    std::string
    EnumType::ToString(size_t v) const
    {
        ASSERT(v < nextIndex_);
        auto f = valueToName_.find(v);
        if(f != valueToName_.end())
        {
            return f->second;
        }

        // ASSERT(false && "Invalid index");
        const auto values = StringMerger::EnglishOr().Generate
        (
            MapToStringVector(nameToValue_)
        );
        const std::string invalid = Str() << "<invalid value " << v << " of "
                                          << values << ">";
        return invalid;
    }


    EnumValue
    EnumType::ToEnum(const std::string& name)
    {
        auto r = nameToValue_.find(name);
        if(r != nameToValue_.end())
        {
            return EnumValue(this, r->second);
        }

        if(!isAdding_)
        {
            LOG_ERROR("Enum value doesnt exist, {0}", name);
            return EnumValue(this, 0);
        }
        const size_t id = nextIndex_;
        AddEnum(name);
        return EnumValue(this, id);
    }


    void
    EnumType::AddEnums(const std::vector<std::string>& names)
    {
        ASSERT(isAdding_);
        std::set<std::string> valid_names;
        for(const auto& name: names)
        {
            AddEnum(name);
            valid_names.insert(name);
        }

        // validate all names against true names
        for(const auto& name: nameToValue_)
        {
            const bool missing
                    = valid_names.find(name.first) == valid_names.end();
            if(missing)
            {
                LOG_ERROR
                (
                    "Enum {0} was registered with name {1} but that is invalid.",
                    name_,
                    name.first
                );
            }
        }

        isAdding_ = false;
    }


    void
    EnumType::AddEnum(const std::string& name)
    {
        ASSERT(isAdding_);
        auto r = nameToValue_.find(name);
        if(r != nameToValue_.end())
        {
            return;
        }
        const size_t id = nextIndex_;
        ++nextIndex_;
        valueToName_.insert(ValueToName::value_type(id, name));
        nameToValue_.insert(NameToValue::value_type(name, id));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Enum Value

    EnumValue::EnumValue(EnumType* t, size_t v) : type_(t), value_(v) {}


    std::string
    EnumValue::ToString() const
    {
        return type_->ToString(value_);
    }


    size_t
    EnumValue::ToValue() const
    {
        return value_;
    }


    bool
    EnumValue::operator==(const EnumValue& other) const
    {
        ASSERT(type_ == other.type_);
        return value_ == other.value_;
    }


    bool
    EnumValue::operator!=(const EnumValue& other) const
    {
        return !(*this == other);
    }


    bool
    EnumValue::operator<(const EnumValue& other) const
    {
        ASSERT(type_ == other.type_);
        return value_ < other.value_;
    }


    std::ostream&
    operator<<(std::ostream& s, const EnumValue& v)
    {
        s << v.ToString();
        return s;
    }


    void
    LoadEnumType
    (
        EnumType* type,
        vfs::FileSystem* fs,
        const vfs::FilePath& path
    )
    {
        ASSERT(type);

        enumlist::Enumroot root;
        const std::string  load_error = LoadProtoJson(fs, &root, path);

        std::vector<std::string> names;

        if(!load_error.empty())
        {
            LOG_ERROR("Failed to load enums {0}: {1}", path, load_error);
        }
        else
        {
            for(const auto& name: root.name)
            {
                names.push_back(name);
            }
        }

        type->AddEnums(names);
    }

}  // namespace euphoria::core
