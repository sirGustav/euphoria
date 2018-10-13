#ifndef EUPHORIA_ENGINE_H
#define EUPHORIA_ENGINE_H

#include <memory>

#include "window/fakesdlevent.h"

class FileSystem;
class SdlLibrary;
class SdlWindow;
class ImguiLibrary;
class SdlGlContext;
class Init;
class ImguiLibrary;
class FileSystemRootCatalog;

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
  std::unique_ptr<FileSystem>            file_system;
  std::shared_ptr<FileSystemRootCatalog> catalog;
  std::unique_ptr<SdlWindow>             window;
  unsigned int                           window_id;
  std::unique_ptr<SdlGlContext>          context;
  std::unique_ptr<Init>                  init;
  std::unique_ptr<ImguiLibrary>          imgui;
};


#endif  // EUPHORIA_ENGINE_H