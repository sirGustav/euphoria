#include "engine/loadworld.h"

#include "core/proto.h"
#include "core/log.h"
#include "core/componentsystem.h"

#include "engine/components.h"
#include "engine/dukregistry.h"

#include "world.pb.h"


LOG_SPECIFY_DEFAULT_LOGGER("engine.loadworld")

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    TextureCache*      cache,
    DukRegistry*       reg,
    const std::string& path)
{
  world::World json;
  const auto   err = LoadProtoJson(fs, &json, path);
  if(!err.empty())
  {
    LOG_ERROR("Failed to load world components from " << path << ": " << err);
  }

  for(const auto& obj : json.objects())
  {
    auto ent = world->reg.create();
    for(const auto& comp : obj.components())
    {
      if(comp.has_position())
      {
        const auto& p                          = comp.position();
        world->reg.assign<CPosition2>(ent).pos = vec2f{p.x(), p.y()};
      }
      else if(comp.has_sprite())
      {
        const auto& s                           = comp.sprite();
        world->reg.assign<CSprite>(ent).texture = cache->GetTexture(s.path());
      }
      else if(comp.has_custom())
      {
        const auto& s = comp.custom();
        ComponentId comp;
        if(reg->GetCustomComponentByName(s.name(), &comp))
        {
          // todo: come up with a better default value
          // perhaps some convert/setup function...
          reg->SetProperty(ent, comp, DukValue());
        }
      }
    }
  }
}
