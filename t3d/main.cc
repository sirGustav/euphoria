#include "core/aabb.h"
#include "core/axisangle.h"
// #include "core/draw.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/mat4.h"
#include "core/os.h"
#include "core/path.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/texturetypes.h"
#include "core/str.h"
#include "core/mat4.h"
#include "core/fpscontroller.h"
#include "core/camera.h"
#include "core/log.h"
#include "core/plane.h"
#include "core/intersection.h"
#include "core/lines.h"

// #include "render/camera.h"
#include "render/defaultfiles.h"
#include "render/materialshadercache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/viewport.h"
#include "render/world.h"
#include "render/compiledmesh.h"
#include "render/debuggl.h"
#include "render/init.h"
#include "render/materialshader.h"
#include "render/texturecache.h"
#include "render/viewporthandler.h"
#include "render/actor.h"
#include "render/compiledlines.h"
#include "render/positionedlines.h"

#include "window/filesystem.h"
#include "window/imgui_ext.h"
#include "window/imguilibrary.h"
#include "window/sdlglcontext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/timer.h"
#include "window/key.h"
#include "window/engine.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>

#include "t3d/tilelibrary.h"

using namespace euphoria::core;
using namespace euphoria::core::vfs;
using namespace euphoria::t3d;
using namespace euphoria::render;
using namespace euphoria::window;

LOG_SPECIFY_DEFAULT_LOGGER("t3d")


struct Editor;

struct Tool
{
  virtual ~Tool() = default;

  virtual void MeshHasChanged(Editor* editor) = 0;
  virtual bool IsBusy(Editor* editor) = 0;
  virtual void Step(Editor* editor) = 0;
  virtual void OnMouse(Editor* editor, MouseButton key, bool down) = 0;
  virtual void OnKey(Editor* editor, Key key, bool down) = 0;
  virtual void OnScroll(Editor* editor, const vec2i& scroll) = 0;
  virtual void OnEditor(Editor* editor) = 0;
};

struct Tools;

struct ToolAction
{
  virtual ~ToolAction() = default;
  virtual void Act(Tools* tools) = 0;
};

struct PushToolAction : public ToolAction
{
  std::shared_ptr<Tool> new_tool;
  void Act(Tools* tools) override;

  explicit PushToolAction(std::shared_ptr<Tool> anew_tool)
    : new_tool(anew_tool)
    {}
};

struct PopToolAction : public ToolAction
{
  void Act(Tools* tools) override;
};

struct Tools
{
  Tool* GetCurrentTool()
  {
    ASSERT(!current_tool.empty());
    Tool* tool = current_tool.rbegin()->get();
    ASSERT(tool);
    return tool;
  }

  void PushTool(std::shared_ptr<Tool> new_tool)
  {
    pending_actions.push_back(std::make_shared<PushToolAction>(new_tool));
  }

  void PopTool()
  {
    pending_actions.push_back(std::make_shared<PopToolAction>());
  }

  void PerformTools()
  {
    for(auto t : pending_actions)
    {
      t->Act(this);
    }
    pending_actions.resize(0);
  }

  std::vector<std::shared_ptr<Tool>> current_tool;
  std::vector<std::shared_ptr<ToolAction>> pending_actions;
};


void PushToolAction::Act(Tools* tools)
  { tools->current_tool.push_back(new_tool); }

void PopToolAction::Act(Tools* tools)
  { tools->current_tool.pop_back(); }


struct Editor
{
  World* world;
  TileLibrary* tile_library;
  CompiledCamera camera;
  Viewport viewport;

  Editor(World* aworld, TileLibrary* atile_library)
    : world(aworld)
    , tile_library(atile_library)
    , camera(mat4f::Identity(), mat4f::Identity())
    , viewport(Recti::FromWidthHeight(10, 10))
  {}

  vec2i mouse = vec2i{0,0};
  Tools tools;

  // current tool callbacks
  void MeshHasChanged()
    { tools.GetCurrentTool()->MeshHasChanged(this); }

  bool IsBusy()
    { return tools.GetCurrentTool()->IsBusy(this); }

  void Step()
    { tools.GetCurrentTool()->Step(this); }

  void OnMouse(MouseButton button, bool down)
    { tools.GetCurrentTool()->OnMouse(this, button, down); }
  void OnKey(Key key, bool down)
    { tools.GetCurrentTool()->OnKey(this, key, down); }
  void OnScroll(const vec2i& scroll)
    { tools.GetCurrentTool()->OnScroll(this, scroll); }
  void OnEditor()
    { tools.GetCurrentTool()->OnEditor(this); }

  // todo(Gustav): move camera here so we can have camera movement
  // change so that fps control rotate focuspoint around current camera pos

  std::vector<std::shared_ptr<Actor>> actors;
  std::shared_ptr<CompiledMesh> selected_mesh;
};

struct NoTool : public Tool
{
  void MeshHasChanged(Editor*) override {}
  bool IsBusy(Editor*) override { return false; }
  void Step(Editor*) override {}

  void OnMouse(Editor*, MouseButton, bool) override {}
  void OnKey(Editor*, Key, bool) override {}
  void OnScroll(Editor*, const vec2i&) override {}
  void OnEditor(Editor*) override
  {
    ImGui::Text("<No tool>");
  }
};

struct PlaceMeshOnPlane : public Tool
{
  std::shared_ptr<Actor> actor;
  Plane plane = Plane::FromNormalAndPoint(unit3f::Up(), vec3f::Zero());

  PlaceMeshOnPlane(std::shared_ptr<Actor> aactor)
    : actor(aactor)
    {}

  void MeshHasChanged(Editor* editor) override
  {
    // todo(Gustav): ugh... fix this ugly hack.. this currently crashes/asserts
    actor->mesh_ = editor->selected_mesh;
  }

  bool IsBusy(Editor*) override { return true;}

  void Step(Editor* editor) override
  {
    auto ray = editor->camera.ClipToWorldRay(editor->viewport.ToClipCoord(editor->mouse)).GetNormalized();
    
    const auto t = GetIntersection(ray, plane);

    // LOG_INFO("Step " << editor->mouse << ", " << t);

    if(t < 0) { return; }
    // do intersection with plane...
    const auto p = ray.GetPoint(t);
    actor->SetPosition(p);
  }

  void OnMouse(Editor* editor, MouseButton button, bool down) override
  {
    if(down) return;
    switch(button)
    {
      case MouseButton::LEFT:
        editor->tools.PopTool();
        break;
    }
  }

  void OnKey(Editor* editor, Key key, bool down) override
  {
    if(down) return;
    switch(key)
    {
      case Key::RETURN:
        editor->tools.PopTool();
        break;
    }
  }
  void OnScroll(Editor*, const vec2i&) override {}  
  void OnEditor(Editor*) override 
  {
    ImGui::Text("Placing object in world!");
  }
};


int
main(int argc, char** argv)
{
  auto engine = Engine{};

  if(engine.Setup(argparse::Args::Extract(argc, argv)) == false)
  {
    return -1;
  }

  int width  = 1280;
  int height = 720;

  if(engine.CreateWindow("t3d", width, height) == false)
  {
    return -1;
  }

  auto viewport_handler = ViewportHandler{};
  viewport_handler.SetSize(width, height);

  auto material_shader_cache = MaterialShaderCache{engine.file_system.get()};

  // SET_ENUM_VALUES(TextureType, SetupTextureNames);
  SET_ENUM_FROM_FILE(engine.file_system.get(), "texture_types.json", TextureType);

  auto texture_cache = TextureCache{engine.file_system.get()};

  auto tile_library = TileLibrary{engine.file_system.get()};
  tile_library.AddDirectory("world", &material_shader_cache, &texture_cache);

  if(tile_library.tiles.empty())
  {
    LOG_ERROR("No tile loaded!");
    return -2;
  }

  auto world = World{};

  auto camera = Camera{};
  camera.position = vec3f(0, 0, 0);

  Editor editor {&world, &tile_library};
  editor.tools.PushTool(std::make_shared<NoTool>());

  bool running = true;

  SdlTimer timer;


  bool immersive_mode = false;

  {
    constexpr auto step = 0.5f;
    constexpr auto size = 10.0f;
    constexpr auto color = Color::Black;
    constexpr auto x_color = Color::PureBlue;
    constexpr auto z_color = Color::PureRed;

    auto def = Lines{};
    for(float x = step; x < size; x+= step)
    {
      def.AddLine(vec3f{x, 0, -size}, vec3f{x, 0, size}, color);
      def.AddLine(vec3f{-size, 0, x}, vec3f{size, 0, x}, color);
      def.AddLine(vec3f{-x, 0, -size}, vec3f{-x, 0, size}, color);
      def.AddLine(vec3f{-size, 0, -x}, vec3f{size, 0, -x}, color);
    }

    def.AddLine(vec3f{-size, 0, 0}, vec3f{size, 0, 0}, x_color);
    def.AddLine(vec3f{0, 0, -size}, vec3f{0, 0, size}, z_color);

    auto compiled = Compile(&material_shader_cache, def);
    auto grid = std::make_shared<PositionedLines>(compiled);
    world.AddActor(grid);
  }

  

  FpsController fps;
  fps.position = vec3f{0, 0, 3};
  engine.window->EnableCharEvent(!immersive_mode);

  bool enviroment_window = false;
  bool camera_window = false;
  bool tiles_window = true;

  while(running)
  {
    const bool  show_imgui = !immersive_mode;
    const float delta      = timer.Update();

    editor.tools.PerformTools();

    {
      SDL_Event e;
      while(SDL_PollEvent(&e) != 0)
      {
        int window_width = 0;
        int window_height = 0;
        if(engine.HandleResize(e, &window_width, &window_height))
        {
          viewport_handler.SetSize(window_width, window_height);
        }
        if(show_imgui)
        {
          engine.imgui->ProcessEvents(&e);
        }

        auto& io = ImGui::GetIO();
        const auto forward_keyboard = !(show_imgui && io.WantCaptureKeyboard);
        const auto forward_mouse = !(show_imgui && io.WantCaptureMouse);

        switch(e.type)
        {
          case SDL_QUIT:
            running = false;
            break;
          case SDL_MOUSEMOTION:
            if(forward_mouse)
            {
              editor.mouse = vec2i(e.motion.x, viewport_handler.height - e.motion.y);
            }
            if(!show_imgui)
            {
              fps.Look(e.motion.xrel, e.motion.yrel);
            }
            break;
          case SDL_KEYDOWN:
          case SDL_KEYUP:
          {
            const bool down = e.type == SDL_KEYDOWN;

            if(!show_imgui)
            {
              fps.HandleKey(ToKey(e.key.keysym), down);
            }

            if(forward_keyboard)
            {

              switch(e.key.keysym.sym)
              {
                case SDLK_ESCAPE:
                  if(down)
                  {
                    running = false;
                  }
                  break;
                case SDLK_TAB:
                  if(!down)
                  {
                    immersive_mode = !immersive_mode;
                    engine.window->KeepWithin(immersive_mode);
                    engine.window->EnableCharEvent(!immersive_mode);
                  }
                  break;
                default:
                  if(forward_keyboard)
                  {
                    editor.OnKey(ToKey(e.key.keysym), down);
                  }
                  break;
              }
            }
          }
          break;

          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
            if(forward_mouse)
            {
              const bool down = e.type == SDL_MOUSEBUTTONDOWN;
              editor.OnMouse(ToKey(e.button), down);
            }
            break;
          
          case SDL_MOUSEWHEEL:
            if(forward_mouse)
            {
              editor.OnScroll(vec2i(e.wheel.x, e.wheel.y));
            }
            break;



          default:
            // ignore other events
            break;
        }
      }

      fps.Update(delta);
    }
      
    editor.Step();

    if(show_imgui)
    {
      engine.imgui->StartNewFrame();

      if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", "Ctrl+Q"))
                {
                    running = false;
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Enviroment", nullptr, &enviroment_window);
                ImGui::MenuItem("Camera", nullptr, &camera_window);
                ImGui::MenuItem("Tiles", nullptr, &tiles_window);
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);

      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
      if(enviroment_window)
      {
        ImGui::Begin("Enviroment", &enviroment_window);
        ImGui::Combo(
            "Type",
            reinterpret_cast<int*>(world.light.GetTypeMod()),
            "Directional\0Point\0Spot\0\0");
        ImGuiColorEdit3("Ambient", world.light.ModifyAmbient());
        ImGuiColorEdit3("Diffuse", world.light.ModifyDiffuse());
        ImGuiColorEdit3("Specular", world.light.ModifySpecular());
        ImGui::End();
      }

      if(camera_window)
      {
        ImGui::Begin("Camera", &camera_window);
        ImGui::DragFloat("Speed", &fps.speed, 0.1f, 0.001f, 10.0f);
        ImGui::End();
      }


      // ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
      if(tiles_window)
      {
        ImGui::Begin("Tiles", &tiles_window);

        if(!tile_library.tiles.empty())
        {
          ImGui::ListBoxHeader("Tiles");
          for(auto tile : tile_library.tiles)
          {
            std::string display = Str{} << tile->name << ": "
                                        << tile->aabb.GetSize();
            if(ImGui::Selectable(display.c_str(), editor.selected_mesh == tile->mesh))
            {
              editor.selected_mesh = tile->mesh;
              editor.MeshHasChanged();
            }
          }
          ImGui::ListBoxFooter();

          if(ImGui::Button("Add"))
          {
            if(editor.selected_mesh && !editor.IsBusy() )
            {
              auto actor = std::make_shared<Actor>(editor.selected_mesh);
              world.AddActor(actor);
              editor.actors.emplace_back(actor);

              editor.tools.PushTool(std::make_shared<PlaceMeshOnPlane>(actor));
            }
          }
        }
        editor.OnEditor();
        ImGui::End();
      }
    }


    camera.position = fps.position;
    camera.rotation = fps.GetRotation();

    {
      auto viewport = viewport_handler.GetFullViewport();
      engine.init->ClearScreen(Color::LightGray);
      editor.camera = camera.Compile(viewport.GetAspectRatio());
      editor.viewport = viewport;
      world.Render(viewport, camera);
    }

    if(show_imgui)
    {
      engine.imgui->Render();
    }

    SDL_GL_SwapWindow(engine.window->window);
  }

  return 0;
}
