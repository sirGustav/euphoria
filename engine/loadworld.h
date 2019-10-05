#ifndef EUPHORIA_LOADWORLD_H
#define EUPHORIA_LOADWORLD_H

#include <string>

namespace euphoria::core
{
  class World;
}

namespace euphoria::core::vfs
{
  class FileSystem;
}

namespace euphoria::duk
{
  class Context;
  class Duk;
}

namespace euphoria::engine
{
  class DukRegistry;
  class ObjectCreator;

  void
  LoadWorld(
      core::vfs::FileSystem*        fs,
      core::World*             world,
      DukRegistry*       reg,
      const std::string& path,
      ObjectCreator*     creator,
      duk::Context*      ctx,
      duk::Duk*          duk);
}

#endif  // EUPHORIA_LOADWORLD_H
