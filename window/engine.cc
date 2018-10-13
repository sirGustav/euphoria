#include "window/engine.h"

#include "SDL.h"
#include "imgui/imgui.h"

#include "core/os.h"
#include "core/log.h"
#include "core/filesystem.h"
#include "core/filesystemdefaultshaders.h"
#include "core/filesystemimagegenerator.h"

#include "render/defaultfiles.h"
#include "render/debuggl.h"
#include "render/init.h"

#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/imguilibrary.h"

LOG_SPECIFY_DEFAULT_LOGGER("window.engine")

Engine::~Engine()
{
  imgui.reset();
  init.reset();
  context.reset();
  window.reset();
  catalog.reset();
  file_system.reset();
  sdl.reset();
}

bool
Engine::Setup()
{
  sdl = std::make_unique<SdlLibrary>();
  if(sdl->ok == false)
  {
    LOG_ERROR("Failed to create SDL");
    return false;
  }

  const auto current_directory = GetCurrentDirectory();

  file_system.reset(new FileSystem{});
  catalog = FileSystemRootCatalog::AddRoot(file_system.get());
  FileSystemRootFolder::AddRoot(file_system.get(), current_directory);
  FileSystemImageGenerator::AddRoot(file_system.get(), "img-plain");
  FileSystemDefaultShaders::AddRoot(file_system.get(), "shaders");

  SetupDefaultFiles(catalog);

  return true;
}

bool
Engine::CreateWindow(
    const std::string& title, int width, int height, bool blend_hack)
{
  const auto pref_path = GetPrefPath();

  window = std::make_unique<SdlWindow>(title, width, height, true);

  if(window->window == nullptr)
  {
    LOG_ERROR("Failed to create window " << SDL_GetError());
    return false;
  }

  window_id = SDL_GetWindowID(window->window);

  context.reset(new SdlGlContext{window.get()});

  if(context->context == nullptr)
  {
    LOG_ERROR("Failed to create SDL context " << SDL_GetError());
    return false;
  }

  init.reset(new Init{
      SDL_GL_GetProcAddress,
      blend_hack ? Init::BlendHack::EnableHack : Init::BlendHack::NoHack});

  if(init->ok == false)
  {
    LOG_ERROR("Failed to create Init");
    return false;
  }

  SetupOpenglDebug();

  imgui.reset(new ImguiLibrary{window->window, pref_path});
  ImGui::StyleColorsLight();

  return true;
}

bool
Engine::HandleResize(SDL_Event e, int* width, int* height)
{
  if(e.type == SDL_WINDOWEVENT)
  {
    if(e.window.windowID == window_id)
    {
      switch(e.window.event)
      {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          SDL_GetWindowSize(window->window, width, height);
          return true;
        default:
          break;
      }
    }
  }

  return false;
}