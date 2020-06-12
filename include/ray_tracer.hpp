#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "scene_parser.hpp"
#include "ray.hpp"
#include "hit.hpp"

#define EPSILON 0.001

class SceneParser;

class RayTracer
{
public:
    RayTracer() = delete;

    RayTracer(SceneParser *scene, int max_bounces, bool shadow, bool reflect,
              bool refract) : scene(scene), max_bounces(max_bounces),
                              use_shadow(shadow), use_reflect(reflect), use_refract(refract)
    {
    }

    ~RayTracer() = default;

    Vector3f traceRay(const Ray &ray, float t_min, int bounces, Hit &hit) const
    {
        hit = Hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        Vector3f finalColor = Vector3f::ZERO;

        bool intersected = scene->getGroup()->intersect(ray, hit, 0);
        if (intersected)
        {
            for (int li = 0; li < scene->getNumLights(); li++)
            {
                Light *light = scene->getLight(li);
                Vector3f L, lightColor;
                float distance;
                light->getIllumination(ray.pointAtParameter(hit.getT()), L, lightColor, distance);
                finalColor += hit.getMaterial()->Shade(ray, hit, L, lightColor);
            }
            if (bounces < max_bounces)
            {
                // TODO: reflection and refraction
                if (use_refract)
                {
                }
                if (use_reflect)
                {
                }
            }
        }
        else
        {
            finalColor = scene->getBackgroundColor();
        }
        return finalColor;
    }

private:
    SceneParser *scene;
    int max_bounces;
    bool use_shadow;
    bool use_reflect;
    bool use_refract;

    Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
    {
        return Vector3f();
    }

    bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                              float index_n, float index_nt,
                              Vector3f &transmitted)
    {
        return false;
    }
};

#endif // RAY_TRACER_H
