#ifndef EUPHORIA_INTERSECTION_H
#define EUPHORIA_INTERSECTION_H

#include "core/ray.h"
#include "core/aabb.h"

struct RayIntersectionResult
{
  bool  intersected;
  float start;
  float end;
};

RayIntersectionResult
Intersect(const UnitRay3f& r, const Aabb& aabb);


#endif  // EUPHORIA_INTERSECTION_H