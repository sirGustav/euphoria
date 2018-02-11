#ifndef SPACETYPER_PROTO_H_
#define SPACETYPER_PROTO_H_

#include <string>

namespace google
{
  namespace protobuf
  {
    class Message;
  }  // namespace protobuf
}  // namespace google

class FileSystem;

bool
LoadProtoText(google::protobuf::Message* message, const std::string& file_name);
bool
SaveProtoText(
    const google::protobuf::Message& message, const std::string& file_name);

bool
LoadProtoBinary(
    google::protobuf::Message* message, const std::string& file_name);
bool
SaveProtoBinary(
    const google::protobuf::Message& message, const std::string& file_name);

// return error message or empty
std::string
LoadProtoJson(
    FileSystem*                fs,
    google::protobuf::Message* message,
    const std::string&         file_name);

std::string
SaveProtoJson(
    const google::protobuf::Message& message, const std::string& file_name);

#endif  // SPACETYPER_PROTO_H_
