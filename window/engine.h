#ifndef EUPHORIA_ENGINE_H
#define EUPHORIA_ENGINE_H

#include <memory>
#include <string>

#include "window/fakesdlevent.h"


namespace euphoria::core
{
  namespace vfs
  {
    class FileSystem;
    class FileSystemRootCatalog;
  }
}

namespace euphoria::render
{
  class Init;
}

namespace euphoria::window
{
  class SdlLibrary;
  class SdlWindow;
  class ImguiLibrary;
  class SdlGlContext;
  class ImguiLibrary;

  struct Engine
  {
    Engine() = default;
    ~Engine();

    bool
    Setup();

    bool
    CreateWindow(
        const std::string& title, int width, int height, bool blend_hack = false);

    bool
    HandleResize(SDL_Event e, int* width, int* height);

    std::unique_ptr<SdlLibrary>            sdl;
    std::unique_ptr<core::vfs::FileSystem>            file_system;
    std::shared_ptr<core::vfs::FileSystemRootCatalog> catalog;
    std::unique_ptr<SdlWindow>             window;
    unsigned int                           window_id;
    std::unique_ptr<SdlGlContext>          context;
    std::unique_ptr<render::Init>                  init;
    std::unique_ptr<ImguiLibrary>          imgui;
  };
}


#endif  // EUPHORIA_ENGINE_H
