#include "core/entity.h"

#include <algorithm>

#include "core/stdutils.h"

////////////////////////////////////////////////////////////////////////////////

static constexpr auto     Shift       = 24;
static constexpr EntityId EntityMask  = 0x00FFFFFF;
static constexpr EntityId VersionMask = 0xFF000000;

////////////////////////////////////////////////////////////////////////////////

#if DEBUG_COMPONENT == 1
Component::Component(const std::type_info& t)
    : type(t)
{
}
#endif

////////////////////////////////////////////////////////////////////////////////

EntityId
GetValue(EntityId id)
{
  return id & EntityMask;
}

EntityVersion
GetVersion(EntityVersion id)
{
  return static_cast<EntityVersion>(id & VersionMask) >> Shift;
}

EntityId
GetId(EntityId id, EntityVersion version)
{
  const EntityId masked_id      = id & EntityMask;
  const EntityId masked_version = (version << Shift) & VersionMask;
  return masked_id | masked_version;
}

////////////////////////////////////////////////////////////////////////////////

struct ComponentList
{
  std::string name;
  std::map<EntityId, std::shared_ptr<Component>> components;

  explicit ComponentList(const std::string& n)
      : name(n)
  {
  }

  std::shared_ptr<Component>
  GetComponent(EntityId entity)
  {
    return components[entity];
  }

  void
  AddComponent(EntityId entity, std::shared_ptr<Component> data)
  {
    components[entity] = data;
  }

  void
  RemoveComponent(EntityId entity)
  {
    components.erase(entity);
  }

  std::vector<EntityId>
  View()
  {
    const auto keys = Keys(components);
    ASSERT(Sorted(keys) == keys);
    return keys;
  }
};

////////////////////////////////////////////////////////////////////////////////

struct RegistryImpl
{
  EntityId              current = 0;
  std::vector<EntityId> free_entities;
  std::vector<EntityId> alive;

  EntityId
  Create()
  {
    if(free_entities.empty())
    {
      current += 1;
      SetAlive(current);
      return current;
    }
    else
    {
      EntityId last = *free_entities.rbegin();
      free_entities.pop_back();
      const auto n = GetId(GetValue(last), GetVersion(last) + 1);
      SetAlive(n);
      return n;
    }
  }

  void
  SetAlive(EntityId a)
  {
    alive.emplace_back(a);
  }

  bool
  IsAlive(EntityId id) const
  {
    return std::find(alive.begin(), alive.end(), id) != alive.end();
  }

  void
  DestroyEntity(EntityId id)
  {
    // todo: postpone!
    if(SwapBackAndEraseObject(id, &alive))
    {
      for(const auto& entry : components)
      {
        entry.second->RemoveComponent(id);
      }
      free_entities.emplace_back(id);
    }
  }

  ComponentId component = 0;
  std::map<ComponentId, std::shared_ptr<ComponentList>> components;
  std::map<std::string, ComponentId>                    name_to_component;

  ComponentId
  NewComponentType(const std::string& name)
  {
    const auto ret = component;
    component += 1;
    components[ret]         = std::make_shared<ComponentList>(name);
    name_to_component[name] = ret;

    return ret;
  }

  std::shared_ptr<Component>
  GetComponent(EntityId entity, ComponentId component)
  {
    return components[component]->GetComponent(entity);
  }

  void
  AddComponent(
      EntityId entity, ComponentId component, std::shared_ptr<Component> data)
  {
    components[component]->AddComponent(entity, data);
  }


  std::vector<EntityId>
  View(const std::vector<ComponentId>& componentList)
  {
    ASSERT(!componentList.empty());
    bool                  first = true;
    std::vector<EntityId> r;
    for(const auto c : componentList)
    {
      const auto v = components[c]->View();
      if(first)
      {
        r     = v;
        first = false;
      }
      else
      {
        std::vector<EntityId> rr;
        std::set_intersection(
            v.begin(), v.end(), r.begin(), r.end(), std::back_inserter(rr));
        r = rr;
      }
    }

    return r;
  }

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id)
  {
    ASSERT(id);
    const auto found_component = name_to_component.find(name);
    if(found_component == name_to_component.end())
    {
      return false;
    }
    else
    {
      *id = found_component->second;
      return true;
    }
  }


  void
  AddAdded()
  {
    // todo: implement
  }

  void
  RemoveRemoved()
  {
  }
};

////////////////////////////////////////////////////////////////////////////////

Registry::Registry()
    : impl(new RegistryImpl{})
{
}

Registry::~Registry()
{
}

EntityId
Registry::Create()
{
  return impl->Create();
}

bool
Registry::IsAlive(EntityId id) const
{
  return impl->IsAlive(id);
}

void
Registry::DestroyEntity(EntityId id)
{
  impl->DestroyEntity(id);
}


ComponentId
Registry::NewComponentType(const std::string& name)
{
  return impl->NewComponentType(name);
}

bool
Registry::GetCustomComponentByName(const std::string& name, ComponentId* id)
{
  return impl->GetCustomComponentByName(name, id);
}

std::shared_ptr<Component>
Registry::GetComponent(EntityId entity, ComponentId component)
{
  return impl->GetComponent(entity, component);
}

void
Registry::AddComponent(
    EntityId entity, ComponentId component, std::shared_ptr<Component> data)
{
  impl->AddComponent(entity, component, data);
}

std::vector<EntityId>
Registry::View(const std::vector<ComponentId>& components)
{
  return impl->View(components);
}

void
Registry::AddAdded()
{
  impl->AddAdded();
}

void
Registry::RemoveRemoved()
{
  impl->RemoveRemoved();
}


////////////////////////////////////////////////////////////////////////////////
