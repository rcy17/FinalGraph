#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "scene_parser.hpp"
#include "ray.hpp"
#include "hit.hpp"

class SceneParser;

class RayTracer
{
public:
  RayTracer() = delete;

  RayTracer(SceneParser *scene, int max_bounces //more arguments as you need...
  );
  ~RayTracer();

  Vector3f traceRay(Ray &ray, float tmin, int bounces,
                    float refr_index, Hit &hit) const;

private:
  SceneParser *m_scene;

  int m_maxBounces;
};

#endif // RAY_TRACER_H
