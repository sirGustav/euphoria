#include <SDL2/SDL.h>
#include <iostream>
#include <core/mat4.h>
#include <render/init.h>

void SetupSdlOpenGlAttributes() {
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}

class SdlTimer {
 public:
  SdlTimer()
      : current_time_(SDL_GetPerformanceCounter())
      , last_time_(0)
  { }

  float Update() {
    last_time_ = current_time_;
    current_time_ = SDL_GetPerformanceCounter();
    const float dt = (current_time_ - last_time_) * 1.0f / SDL_GetPerformanceFrequency();
    return dt;
  }

 private:
  Uint64 current_time_;
  Uint64 last_time_;
};

int main(int argc, char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
    std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
    return -1;
  }

  SetupSdlOpenGlAttributes();

  int width = 800;
  int height = 600;

  SDL_Window* window = SDL_CreateWindow("Euphoria Demo", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    std::cerr << "Failed to create window " << SDL_GetError() << "\n";
    return -1;
  }

  SDL_GLContext context = SDL_GL_CreateContext(window);

  if (context == nullptr) {
    std::cerr << "Failed to create GL context " << SDL_GetError() << "\n";
    return -1;
  }

  Init init;

  if (init.ok == false) {
    return -4;
  }

  // SetupOpenglDebug();

  mat4f projection =
      mat4f::Ortho(0.0f, static_cast<float>(width),
                   static_cast<float>(height), 0.0f, -1.0f, 1.0f);

  bool running = true;

  SdlTimer timer;

  while (running) {
    const float delta = timer.Update();

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    init.ClearScreen();

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
