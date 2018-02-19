#ifndef CORE_FILESYSTEMDEFAULTSHADERS_H
#define CORE_FILESYSTEMDEFAULTSHADERS_H


#include "core/filesystem.h"


class FileSystemDefaultShaders : public FileSystemReadRoot
{
 public:
  static void
  AddRoot(FileSystem* fs, const std::string& base);

  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override;

  std::string
  Describe() override;

  explicit FileSystemDefaultShaders(const std::string& base);

 private:
  std::string base_;
};


#endif  // CORE_FILESYSTEMDEFAULTSHADERS_H