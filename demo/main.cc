#include <core/mat4.h>
#include "core/draw.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/filesystem.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/filesystemimagegenerator.h"

#include <render/init.h>
#include <render/debuggl.h>
#include <render/materialshader.h>
#include <render/compiledmesh.h>
#include <render/texturecache.h>
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/camera.h"
#include "render/materialshadercache.h"

#include "window/imguilibrary.h"
#include "window/timer.h"

#include "imgui/imgui.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>


void
SetupSdlOpenGlAttributes()
{
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
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

class Sdl
{
 public:
  Sdl()
      : ok(false)
  {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
    {
      std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
      return;
    }

    ok = true;
  }

  ~Sdl()
  {
    SDL_Quit();
  }

  bool ok;
};

class SdlWindow
{
 public:
  SdlWindow(const std::string& title, int width, int height)
      : window(nullptr)
  {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if(window == nullptr)
    {
      std::cerr << "Failed to create window " << SDL_GetError() << "\n";
    }
  }

  ~SdlWindow()
  {
    SDL_DestroyWindow(window);
  }

  void
  KeepWithin(bool k)
  {
    // SDL_SetWindowGrab(window, k ? SDL_TRUE : SDL_FALSE);
    // SDL_ShowCursor(k ? SDL_DISABLE : SDL_ENABLE);
    SDL_SetRelativeMouseMode(k ? SDL_TRUE : SDL_FALSE);
  }

  SDL_Window* window;
};

class SdlGlContext
{
 public:
  SdlGlContext(SdlWindow* window)
      : context(nullptr)
  {
    context = SDL_GL_CreateContext(window->window);
    if(context == nullptr)
    {
      std::cerr << "Failed to create GL context " << SDL_GetError() << "\n";
      return;
    }
  }

  ~SdlGlContext()
  {
    SDL_GL_DeleteContext(context);
  }

  SDL_GLContext context;
};

struct CubeAnimation
{
  CubeAnimation()
      : timer(0)
      , from(quatf::Identity())
      , to(quatf::Identity())
      , rotation_speed(1.0f)
      , move_speed(1.0f)
  {
  }

  std::shared_ptr<Actor> actor;
  float                  timer;
  quatf                  from;
  quatf                  to;
  float                  rotation_speed;
  float                  move_speed;
};

// todo: move to a better place
class FpsController
{
 public:
  FpsController();

  void
  Look(float delta_rot, float delta_look);

  void
  MoveLeft(bool down);
  void
  MoveRight(bool down);
  void
  MoveForward(bool down);
  void
  MoveBackward(bool down);
  void
  MoveUp(bool down);
  void
  MoveDown(bool down);

  void
  HandleSdlKey(int key, bool down);

  void
  Update(float delta);

  vec3f
  GetPosition() const;
  void
  SetPosition(const vec3f& pos);
  quatf
  GetRotation() const;

 private:
  Angle rotation_;
  Angle look_;

  vec3f pos_;

  bool left_down_     = false;
  bool right_down_    = false;
  bool forward_down_  = false;
  bool backward_down_ = false;
  bool up_down_       = false;
  bool down_down_     = false;

  float speed_       = 3.0f;
  float sensitivity_ = 0.10f;
};

FpsController::FpsController()
    : rotation_(Angle::Zero())
    , look_(Angle::Zero())
    , pos_(vec3f::Origo())
{
}

void
FpsController::Look(float x, float y)
{
  rotation_ += Angle::FromDegrees(-x * sensitivity_);
  look_ += Angle::FromDegrees(-y * sensitivity_);
}

void
FpsController::MoveLeft(bool down)
{
  left_down_ = down;
}
void
FpsController::MoveRight(bool down)
{
  right_down_ = down;
}
void
FpsController::MoveForward(bool down)
{
  forward_down_ = down;
}
void
FpsController::MoveBackward(bool down)
{
  backward_down_ = down;
}
void
FpsController::MoveUp(bool down)
{
  up_down_ = down;
}
void
FpsController::MoveDown(bool down)
{
  down_down_ = down;
}

void
FpsController::HandleSdlKey(int key, bool down)
{
  switch(key)
  {
    case SDLK_w:
    case SDLK_UP:
      MoveForward(down);
      return;
    case SDLK_s:
    case SDLK_DOWN:
      MoveBackward(down);
      return;
    case SDLK_a:
    case SDLK_LEFT:
      MoveLeft(down);
      return;
    case SDLK_d:
    case SDLK_RIGHT:
      MoveRight(down);
      return;

    case SDLK_SPACE:
      MoveUp(down);
      return;
    case SDLK_LCTRL:
      MoveDown(down);
      return;
  }
}

void
FpsController::Update(float delta)
{
  int forward = 0;
  int right   = 0;
  int up      = 0;

  if(forward_down_)
    forward += 1;
  if(backward_down_)
    forward -= 1;

  if(right_down_)
    right += 1;
  if(left_down_)
    right -= 1;

  if(up_down_)
    up += 1;
  if(down_down_)
    up -= 1;

  if(forward == 0 && right == 0 && up == 0)
    return;

  const vec3f input =
      GetRotation().RightUpIn(vec3f(right, up, forward)).GetNormalized();
  const vec3f movement = input * speed_ * delta;

  pos_ += movement;
}

void
FpsController::SetPosition(const vec3f& pos)
{
  pos_ = pos;
}
vec3f
FpsController::GetPosition() const
{
  return pos_;
}
quatf
FpsController::GetRotation() const
{
  const auto rotation = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(vec3f::YAxis(), rotation_));
  const auto look =
      quatf::FromAxisAngle(AxisAngle::RightHandAround(vec3f::XAxis(), look_));
  return rotation * look;
}

int
main(int argc, char** argv)
{
  Sdl sdl;
  if(sdl.ok == false)
  {
    return -1;
  }

  SetupSdlOpenGlAttributes();

  int width  = 1280;
  int height = 720;

  SdlWindow window{"Euphoria Demo", 1280, 720};
  if(window.window == nullptr)
  {
    return -1;
  }

  SdlGlContext context{&window};

  if(context.context == nullptr)
  {
    return -1;
  }

  Init init;
  if(init.ok == false)
  {
    return -4;
  }

  SetupOpenglDebug();

  ImguiLibrary imgui{window.window};

  Viewport viewport{Recti::FromTopLeftWidthHeight(0, 0, width, height)};
  viewport.Activate();

  FileSystem file_system;
  auto       catalog = FileSystemRootCatalog::AddRoot(&file_system);
  FileSystemRootFolder::AddRoot(&file_system);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");
  catalog->RegisterFileString(
      "default_shader.json",
      R"(  {"has_light": true, "ambient": "uMaterial.ambient", "diffuse": "uMaterial.diffuse", "specular": "uMaterial.specular", "shininess": "uMaterial.shininess", "textures": [ {"texture": "Diffuse", "uniform": "uDiffuseMap"}, {"texture": "Specular", "uniform": "uSpecularMap"} ]}  )");
  catalog->RegisterFileString(
      "default_shader.vert",
      "#version 330 core\n"
      "in vec3 aPosition;\n"
      "in vec3 aNormal;\n"
      "in vec2 aTexCoord;\n"
      "\n"
      "out vec2 texCoord;\n"
      "out vec3 normal;\n"
      "out vec3 fragPositionWorld;\n"
      "\n"
      "uniform mat4 uProjection;\n"
      "uniform mat4 uView;\n"
      "uniform mat4 uModel;\n"
      "uniform mat3 uNormalMatrix;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
      "    fragPositionWorld = vec3(uModel * vec4(aPosition, 1.0));\n"
      "    texCoord = aTexCoord;\n"
      "    normal = uNormalMatrix * aNormal;\n"
      "}\n");
  catalog->RegisterFileString(
      "default_shader.frag",
      "#version 330 core\n"
      "struct Material\n"
      "{\n"
      "  vec3 ambient;\n"
      "  vec3 diffuse;\n"
      "  vec3 specular;\n"
      "  float shininess;\n"
      "};\n"
      "\n"
      "struct Light\n"
      "{\n"
      "  int type;\n"
      "  vec3 ambient;\n"
      "  vec3 diffuse;\n"
      "  vec3 specular;\n"
      "  \n"
      "  float attConst;\n"
      "  float attLin;\n"
      "  float attQuad;\n"
      "  \n"
      "  vec3 position;\n"
      "  vec3 direction;\n"
      "};\n"
      "\n"
      "out vec4 FragColor;\n"
      "\n"
      "in vec2 texCoord;\n"
      "in vec3 normal;\n"
      "in vec3 fragPositionWorld;\n"
      "\n"
      "uniform sampler2D uDiffuseMap;\n"
      "uniform sampler2D uSpecularMap;\n"
      "\n"
      "uniform Light uLight;\n"
      "uniform vec3 uViewPosition;\n"
      "uniform Material uMaterial;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    vec3 ambient = uMaterial.ambient * uLight.ambient;\n"
      "    \n"
      "    vec3 norm = normalize(normal);\n"
      "    vec3 lightDir = uLight.type==0 ? normalize(-uLight.direction) :\n"
      "                    normalize(uLight.position - fragPositionWorld);\n"
      "    float diffuse_factor = max(0.0, dot(norm, lightDir));\n"
      "    vec3 diffuse = diffuse_factor * uMaterial.diffuse * \n"
      "                     uLight.diffuse;\n"
      "    \n"
      "    float specularStrength = 0.5;\n"
      "    vec3 viewDir = normalize(uViewPosition - fragPositionWorld);\n"
      "    vec3 reflectDir = reflect(-lightDir, norm);\n"
      "    float spec = pow(max(0, dot(viewDir, reflectDir)), \n"
      "                   uMaterial.shininess);\n"
      "    vec3 specular = specularStrength * uMaterial.specular * spec * \n"
      "                      uLight.specular;\n"
      "    \n"
      "    if(uLight.type != 0) {\n"
      "      float dist = length(uLight.position - fragPositionWorld);\n"
      "      float att = 1.0 / (uLight.attConst + dist*uLight.attLin + \n"
      "                         dist*dist*uLight.attQuad);\n"
      "      ambient *= att;\n"
      "      diffuse *= att;\n"
      "      specular *= att;\n"
      "    }\n"
      "    \n"
      "    vec3 object_color = texture(uDiffuseMap, texCoord).rgb;\n"
      "    vec3 specular_map = texture(uSpecularMap, texCoord).rgb;\n"
      "    \n"
      "    vec3 result = (ambient + diffuse) * object_color + \n"
      "                    specular * specular_map;\n"
      "    \n"
      "    FragColor = vec4(result, 1.0);\n"
      "}\n");

  catalog->RegisterFileString("basic_shader.json",
                              R"(  {"diffuse": "uDiffuse", "textures": []}  )");
  catalog->RegisterFileString(
      "basic_shader.vert",
      "#version 330 core\n"
      "in vec3 aPosition;\n"
      "\n"
      "\n"
      "uniform mat4 uProjection;\n"
      "uniform mat4 uView;\n"
      "uniform mat4 uModel;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
      "}\n");
  catalog->RegisterFileString("basic_shader.frag",
                              "#version 330 core\n"
                              "\n"
                              "uniform vec3 uDiffuse;\n"
                              "\n"
                              "out vec4 FragColor;\n"
                              "\n"
                              "void main()\n"
                              "{\n"
                              "    FragColor = vec4(uDiffuse, 1.0);\n"
                              "}\n");

  catalog->RegisterFileString("texture_types.json",
                              R"({"name" : ["Diffuse", "Specular"]})");

  MaterialShaderCache material_shader_cache{&file_system};

  // SET_ENUM_VALUES(TextureType, SetupTextureNames);
  SET_ENUM_FROM_FILE(&file_system, "texture_types.json", TextureType);

  Image image;
  image.Setup(256, 256, false);
  Draw       drawer{&image};
  const auto wi = drawer.WholeImage();
  drawer.Clear(Rgb::From(Color::Red));
  Random random{42};

  for(int i = 0; i < 20; i += 1)
  {
    const auto color = Rgb::From(random.NextDawnbringerPalette());
    const auto pos   = random.NextPoint(wi);
    const auto outer = random.NextRange(55.0f, 100.0f);
    const auto inner = random.NextRange(50.0f);
    drawer.Circle(color, pos, outer, 10, inner);
  }
  drawer
      .LineAntialiased(Rgb::From(Color::Black), wi.TopLeft(), wi.BottomRight())
      .LineAntialiased(Rgb::From(Color::Black), wi.BottomLeft(), wi.TopRight())
      .Text(wi.GetAbsoluteCenterPos(), "Hello world", Rgb::From(Color::Black),
            2);
  catalog->RegisterFileData("image", image.Write(ImageWriteFormat::PNG));

  TextureCache texture_cache{&file_system};

  bool running = true;

  SdlTimer timer;

  World world;

  auto box_mesh1 = meshes::CreateCube(0.5f);
  box_mesh1.materials[0].SetTexture("Diffuse", "container2.png");
  box_mesh1.materials[0].SetTexture("Specular", "container2_specular.png");
  box_mesh1.materials[0].ambient =
      Rgb::From(Color::White);  // fix ambient color on material
  box_mesh1.materials[0].specular  = Rgb::From(Color::White);
  box_mesh1.materials[0].shininess = 120.0f;
  auto box1 = CompileMesh(box_mesh1, &material_shader_cache, &texture_cache);

  auto box_mesh2 = meshes::CreateSphere(0.5f, "image");
  box_mesh2.materials[0].SetTexture("Specular", "img-plain/white");
  box_mesh2.materials[0].ambient =
      Rgb::From(Color::White);  // fix ambient color on material
  box_mesh2.materials[0].specular  = Rgb::From(Color::White);
  box_mesh2.materials[0].shininess = 10.0f;
  auto box2 = CompileMesh(box_mesh2, &material_shader_cache, &texture_cache);

  auto light_mesh                = meshes::CreateCube(0.2f);
  light_mesh.materials[0].shader = "basic_shader";
  auto  light = CompileMesh(light_mesh, &material_shader_cache, &texture_cache);
  float light_position = 0.0f;

  const float box_extent_value = 4;
  Aabb        box_extents{
      vec3f{-box_extent_value, -box_extent_value, -box_extent_value},
      vec3f{box_extent_value, box_extent_value, box_extent_value}};

  std::vector<CubeAnimation> animation_handler;

  bool capturing_mouse_movement = false;

  for(int i = 0; i < 20; ++i)
  {
    std::shared_ptr<Actor> actor =
        std::make_shared<Actor>(random.NextBool() ? box1 : box2);
    world.AddActor(actor);

    CubeAnimation anim;
    anim.actor          = actor;
    anim.from           = random.NextQuatf();
    anim.to             = random.NextQuatf();
    anim.rotation_speed = random.NextRange(0.5f, 1.0f);
    anim.move_speed     = random.NextRange(0.5f, 1.0f);
    anim.timer          = random.NextFloat01();


    // generate a position not too close to the center
    vec3f position = vec3f::Origo();
    do
    {
      position = random.NextVec3(box_extents);
    } while(position.GetLength() < 1.4f);

    actor->SetPosition(position);
    actor->SetRotation(anim.from);

    animation_handler.push_back(anim);
  }

  auto light_actor = std::make_shared<Actor>(light);
  world.AddActor(light_actor);
  light_actor->BeginMaterialOverride(0);
  auto* light_material = light_actor->GetOverriddenMaterial(0);

  Camera camera;
  camera.SetPosition(vec3f(0, 0, 0));

  FpsController fps;
  fps.SetPosition(vec3f(0, 0, 3));

  bool paused = true;

  bool show_test_window = true;

  while(running)
  {
    const bool  show_imgui = !capturing_mouse_movement;
    const float delta      = timer.Update();

    if(show_imgui)
    {
      imgui.Begin();

      // 3. Show the ImGui test window. Most of the sample code is in
      // ImGui::ShowTestWindow()
      if(show_test_window)
      {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);

        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Light");
        ImGui::Combo("Type", reinterpret_cast<int*>(world.light.GetTypeMod()),
                     "Directional\0Point\0Spot\0\0");
        ImGui::ColorEdit3("Ambient", world.light.ModifyAmbient()->GetData());
        ImGui::ColorEdit3("Diffuse", world.light.ModifyDiffuse()->GetData());
        ImGui::ColorEdit3("Specular", world.light.ModifySpecular()->GetData());
        ImGui::End();

        light_material->SetColors(world.light.GetAmbient(),
                                  world.light.GetDiffuse(),
                                  world.light.GetSpecular(), 10);
      }
    }

    light_position = Wrap(0, light_position + delta * 0.1f, 1);
    const auto light_pos =
        PolarCoord{light_position, light_position * 2}.ToCartesian() * 2.0f;
    world.light.SetPosition(light_pos);
    light_actor->SetPosition(light_pos);


    for(auto& anim : animation_handler)
    {
      if(paused == false)
      {
        anim.timer += delta * anim.rotation_speed;
        int count = 0;
        while(anim.timer > 1.0f)
        {
          count += 1;
          anim.timer -= 1.0f;
          anim.from           = anim.to;
          anim.to             = random.NextQuatf();
          anim.rotation_speed = random.NextRange(0.3f, 1.0f);
          anim.move_speed     = random.NextRange(0.2f, 3.0f);
        }
        ASSERT(count < 2);
        quatf q = quatf::SlerpShortway(anim.from, anim.timer, anim.to);
        anim.actor->SetRotation(q);
        const vec3f movement = q.In() * anim.move_speed * delta;
        const vec3f new_pos =
            box_extents.Wrap(anim.actor->GetPosition() + movement);
        anim.actor->SetPosition(
            new_pos);  // hard to see movement when everything is moving
      }
    }

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      if(show_imgui)
      {
        imgui.ProcessEvents(&e);
      }
      switch(e.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEMOTION:
          if(capturing_mouse_movement)
          {
            fps.Look(e.motion.xrel, e.motion.yrel);
          }
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
          const bool down = e.type == SDL_KEYDOWN;

          fps.HandleSdlKey(e.key.keysym.sym, down);

          switch(e.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              if(down)
              {
                running = false;
              }
              break;
            case SDLK_p:
              if(!down)
              {
                paused = !paused;
              }
              break;
            case SDLK_TAB:
              if(!down)
              {
                capturing_mouse_movement = !capturing_mouse_movement;
                window.KeepWithin(capturing_mouse_movement);
              }
              break;
            default:
              // ignore other keys
              break;
          }
        }
        break;
        default:
          // ignore other events
          break;
      }
    }

    fps.Update(delta);
    camera.SetPosition(fps.GetPosition());
    camera.SetRotation(fps.GetRotation());

    init.ClearScreen(Rgb::From(Color::DarkslateGray));
    world.Render(viewport, camera);

    if(show_imgui)
    {
      imgui.Render();
    }

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}

#if 0
class View {
Camera[] cameras;
};

class CameraView {
Camera;
ViewPort area; // glViewport
};

class Camera {
Mode mode;
World world;
culling info from world, created when camera is added to world
viewMatrix; // position + rotation
projectionMatrix; // ortho/perspective

// 4 planes from the projection matrix
getFrustum();

toViewSpace(); // camera space, eye space
toWorldSpace();
};

// normal, thermo optics/powerup
class Mode {
Pass[] passes;
};

class Pass {
tag/list
Shader shader;
};

class Shader {
texture list: list of texture names mapped to shader variables
};

class World {
Actor[] objects;
};

class Actor {
Mesh mesh;
position;
rotation;
modelMatrix;
ToLocalSpace();
ToWorldSpace();
};

class Mesh {
MeshPart[] meshParts;
};

class MeshPart {
Material material;
};

class MaterialList {
maps id, material name, shader variable
};

class Material {
Texture[] textures;
};
#endif
