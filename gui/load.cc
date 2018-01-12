#include "gui/load.h"

#include <iostream>
#include <map>

#include "core/proto.h"
#include "core/log.h"

#include "render/texturecache.h"
#include "render/scalablesprite.h"
#include "render/fontcache.h"

#include "gui/layoutcontainer.h"
#include "gui/layout.h"
#include "gui/button.h"
#include "gui/panelwidget.h"
#include "gui/skin.h"
#include "gui/root.h"

#include "gui.pb.h"

LOG_SPECIFY_DEFAULT_LOGGER("gui.load")

template <typename T>
std::vector<T>
ToVector(const google::protobuf::RepeatedField<T>& a)
{
  std::vector<T> r;
  for(const T& t : a)
  {
    r.push_back(t);
  }
  return r;
}

std::shared_ptr<Layout>
GetLayout(const gui::Layout& c)
{
  if(c.has_table())
  {
    LOG_INFO("Creating a table layout");
    return CreateTableLayout(
        ToVector(c.table().expanded_rows()),
        ToVector(c.table().expanded_cols()),
        c.table().padding());
  }
  else
  {
    LOG_INFO("Creating a single row layout");
    return CreateSingleRowLayout(c.single_row().padding());
  }
}

class CmdButton : public Button
{
 public:
  CmdButton(UiState* state)
      : Button(state)
  {
  }
  void
  OnClicked() override
  {
    std::cout << "Executing cmd: " << cmd << "\n";
  }
  std::string cmd;
};

void
BuildLayoutContainer(
    UiState*                    state,
    LayoutContainer*            root,
    const gui::LayoutContainer& c,
    TextureCache*               cache,
    const std::map<std::string, Skin*>& skins);

void
SetupLayout(LayoutData* data, const gui::Widget& src)
{
  data->SetColumn(src.column());
  data->SetRow(src.row());
  data->SetPreferredWidth(src.preferred_width());
  data->SetPreferredHeight(src.preferred_height());
}

Lrtb
LrtbFromProt(const gui::Lrtb& lrtd)
{
  Lrtb r;
  r.left   = lrtd.left();
  r.right  = lrtd.right();
  r.top    = lrtd.top();
  r.bottom = lrtd.bottom();
  return r;
}

std::shared_ptr<Widget>
CreateWidget(
    UiState*           state,
    const gui::Widget& w,
    TextureCache*      cache,
    const std::map<std::string, Skin*>& skins)
{
  std::shared_ptr<Widget> ret;

  if(w.has_button())
  {
    LOG_INFO("Creating a button widget");
    CmdButton* b = new CmdButton(state);

    const std::string skin_name = w.button().skin();
    const auto        skin_it   = skins.find(skin_name);
    if(skin_it != skins.end())
    {
      b->SetSkin(skin_it->second);
    }
    else
    {
      std::cerr << "Failed to find skin " << skin_name << "\n";
    }
    Skin* skin = skin_it->second;

    if(!skin_it->second->button_image.empty())
    {
      std::shared_ptr<ScalableSprite> sp(new ScalableSprite(
          skin->button_image, Sizef::FromSquare(2.0f), cache));
      b->SetSprite(sp);
    }
    ret.reset(b);
    b->cmd = w.button().command();
    b->Text().SetString(w.button().text());
    b->Text().SetFont(skin->font);
  }
  else
  {
    LOG_INFO("Creating a panel widget");
    PanelWidget* l = new PanelWidget(state);
    ret.reset(l);
    BuildLayoutContainer(
        state, &l->container, w.panel().container(), cache, skins);
  }

  ASSERT(ret.get());

  // load basic widget data
  ret->name    = w.name();
  ret->padding = LrtbFromProt(w.padding());
  ret->margin  = LrtbFromProt(w.margin());

  SetupLayout(&ret->layout, w);

  return ret;
}

void
BuildLayoutContainer(
    UiState*                    state,
    LayoutContainer*            root,
    const gui::LayoutContainer& c,
    TextureCache*               cache,
    const std::map<std::string, Skin*>& skins)
{
  root->SetLayout(GetLayout(c.layout()));
  for(const gui::Widget& widget : c.widgets())
  {
    root->Add(CreateWidget(state, widget, cache, skins));
  }
}

Rgb
Load(const gui::Rgb& src)
{
  return Rgb(src.r(), src.g(), src.b());
}

InterpolationType
Load(gui::InterpolationType t)
{
  switch(t)
  {
#define FUN(NAME, FUNC) \
  case gui::NAME:       \
    return InterpolationType::NAME;
    // Linear interpolation (no easing)
    FUN(Linear, LinearInterpolation)

    // Quadratic easing; p^2
    FUN(QuadIn, QuadraticEaseIn)
    FUN(QuadOut, QuadraticEaseOut)
    FUN(Quad, QuadraticEaseInOut)

    // Cubic easing; p^3
    FUN(CubicIn, CubicEaseIn)
    FUN(CubicOut, CubicEaseOut)
    FUN(Cubic, CubicEaseInOut)

    // Quartic easing; p^4
    FUN(QuartIn, QuarticEaseIn)
    FUN(QuartOut, QuarticEaseOut)
    FUN(Quart, QuarticEaseInOut)

    // Quintic easing; p^5
    FUN(QuintIn, QuinticEaseIn)
    FUN(QuintOut, QuinticEaseOut)
    FUN(Quint, QuinticEaseInOut)

    // Sine wave easing; sin(p * PI/2)
    FUN(SineIn, SineEaseIn)
    FUN(SineOut, SineEaseOut)
    FUN(Sine, SineEaseInOut)

    // Circular easing; sqrt(1 - p^2)
    FUN(CircIn, CircularEaseIn)
    FUN(CircOut, CircularEaseOut)
    FUN(Circ, CircularEaseInOut)

    // Exponential easing, base 2
    FUN(ExpIn, ExponentialEaseIn)
    FUN(ExpOut, ExponentialEaseOut)
    FUN(Exp, ExponentialEaseInOut)

    // Exponentially-damped sine wave easing
    FUN(ElasticIn, ElasticEaseIn)
    FUN(ElasticOut, ElasticEaseOut)
    FUN(Elastic, ElasticEaseInOut)

    // Overshooting cubic easing;
    FUN(BackIn, BackEaseIn)
    FUN(BackOut, BackEaseOut)
    FUN(Back, BackEaseInOut)

    // Exponentially-decaying bounce easing
    FUN(BounceIn, BounceEaseIn)
    FUN(BounceOut, BounceEaseOut)
    FUN(Bounce, BounceEaseInOut)
#undef FUN
    default:
      return InterpolationType::INVALID;
  }
}

ButtonState
LoadButton(const gui::ButtonState& src)
{
  ButtonState ret;
  // ret.image                 = src.image();
  ret.scale                 = src.scale();
  ret.image_color           = Load(src.image_color());
  ret.text_color            = Load(src.text_color());
  ret.dx                    = src.dx();
  ret.dy                    = src.dy();
  ret.interpolationColor    = Load(src.interpolate_color());
  ret.interpolationSize     = Load(src.interpolate_size());
  ret.interpolationPosition = Load(src.interpolate_position());

  ret.interpolationColorTime    = src.interpolate_color_time();
  ret.interpolationSizeTime     = src.interpolate_size_time();
  ret.interpolationPositionTime = src.interpolate_position_time();
  return ret;
}

std::shared_ptr<Skin>
LoadSkin(const gui::Skin& src, FontCache* font)
{
  std::shared_ptr<Skin> skin(new Skin());
  skin->name              = src.name();
  skin->font              = font->GetFont(src.font());
  skin->button_image      = src.button_image();
  skin->button_idle       = LoadButton(src.button_idle());
  skin->button_hot        = LoadButton(src.button_hot());
  skin->button_active_hot = LoadButton(src.button_active_hot());
  return skin;
}

bool
Load(
    Root*              root,
    FileSystem*        fs,
    FontCache*         font,
    const std::string& path,
    TextureCache*      cache)
{
  gui::File         f;
  const std::string load_result = LoadProtoJson(fs, &f, path);
  if(false == load_result.empty())
  {
    std::cerr << "Failed to load gui from " << path << ": " << load_result
              << "\n";
    return false;
  }

  root->cursor_image = cache->GetTextureIfNotEmpty(f.cursor_image());
  root->hover_image  = cache->GetTextureIfNotEmpty(f.hover_image());

  std::map<std::string, Skin*> skin_map;

  for(const gui::Skin& skin : f.skins())
  {
    std::shared_ptr<Skin> skin_ptr = LoadSkin(skin, font);
    skin_map.insert(std::make_pair(skin.name(), skin_ptr.get()));
    root->skins_.push_back(skin_ptr);
  }

  BuildLayoutContainer(
      &root->state_, &root->container_, f.root(), cache, skin_map);

  return root->container_.HasWidgets();
}
