#include "ray_tracer.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "group.hpp"
#include "material.hpp"
#include "light.hpp"

#define EPSILON 0.001

// TODO: IMPLEMENT THESE FUNCTIONS
// These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float index_n, float index_nt,
                          Vector3f &transmitted)
{
}

RayTracer::RayTracer(SceneParser *scene, int max_bounces
                     //more arguments if you need...
                     ) : m_scene(scene)

{
  // TODO
  auto g = scene->getGroup();
  m_maxBounces = max_bounces;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay(Ray &ray, float tmin, int bounces,
                             float refr_index, Hit &hit) const
{
  hit = Hit(FLT_MAX, NULL, Vector3f(0, 0, 0));

  return Vector3f(0, 0, 0);
}
