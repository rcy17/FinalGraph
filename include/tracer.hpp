#ifndef TRACER_H
#define TRACER_H

#include <cassert>
#include <vector>
#include "scene_parser.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "vector_utils.hpp"
#define EPSILON 0.00001f

class SceneParser;

inline Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{

    return incoming - 2 * Vector3f::dot(incoming, normal) * normal;
}

inline bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                                 double index_n, double index_nt,
                                 Vector3f &transmitted)
{
    auto dn = Vector3f::dot(incoming, normal);
    double coff = dn > 0 ? -1 : 1;
    auto delta = 1 - index_n * index_n * (1 - dn * dn) / (index_nt * index_nt);
    if (delta < 0)
        return false;
    transmitted = index_n * (incoming - dn * normal) / index_nt - coff * normal * sqrt(delta);
    return true;
}

class Tracer
{
public:
    Tracer() = delete;

    Tracer(SceneParser *scene, int max_bounces, bool shadow,
           bool refract) : scene(scene), max_bounces(max_bounces),
                           use_shadow(shadow), use_refract(refract)
    {
    }

    virtual ~Tracer() = default;

    virtual Vector3f traceRay(const Ray &ray, double t_min, int bounces, Hit &hit, double currentIndex = 1.f, bool debug = false) const = 0;

protected:
    SceneParser *scene;
    int max_bounces;
    bool use_shadow;
    bool use_refract;
};

#endif // TRACER_H
