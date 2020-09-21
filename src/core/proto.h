#ifndef SPACETYPER_PROTO_H_
#define SPACETYPER_PROTO_H_

#include <string>
#include "rapidjson/document.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }

    std::string
    LoadProtoJson_Internal
    (
        vfs::FileSystem* fs,
        rapidjson::Document* doc,
        const vfs::FilePath& file_name
    );

    std::string
    ParseJsonSource(const std::string& source, rapidjson::Document* doc);

    // return error message or empty
    template <typename T>
    std::string
    LoadProtoJson(vfs::FileSystem* fs, T* message, const vfs::FilePath& file_name)
    {
        rapidjson::Document doc;
        const std::string   r = LoadProtoJson_Internal(fs, &doc, file_name);
        if(r.empty())
        {
            return ReadFromJsonValue(message, doc, "");
        }
        else
        {
            return r;
        }
    }


}  // namespace euphoria::core

#endif  // SPACETYPER_PROTO_H_