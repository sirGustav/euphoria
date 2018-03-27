#ifndef EUPHORIA_INPUT_H
#define EUPHORIA_INPUT_H

#include <string>
#include <memory>

#include "core/key.h"

#include "engine/duk.h"
#include "dukglue/dukglue.h"

struct BoundVar
{
  std::string name;
  float       value;
  Key         key;

  BoundVar(const std::string& n, const Key& k);

  // todo: figure out how to bind keys to this property
  // todo: figure out how to handle just pressed
  // todo: figure out how to handle player characters
  // todo: figure out how to handle inputs other than keyboard
};

class Input
{
 public:
  void
  Add(std::shared_ptr<BoundVar> bind);

  void
  SetKeyState(Key key, float state);

  void
  Set(Duk* duk, DukValue container) const;

 private:
  std::vector<std::shared_ptr<BoundVar>> binds;
};


#endif  // EUPHORIA_INPUT_H