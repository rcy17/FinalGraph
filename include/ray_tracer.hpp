#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "scene_parser.hpp"
#include "ray.hpp"
#include "hit.hpp"
#define EPSILON 0.001f

class SceneParser;

inline Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{

    return incoming - 2 * Vector3f::dot(incoming, normal) * normal;
}

inline bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                                 float index_n, float index_nt,
                                 Vector3f &transmitted)
{
    auto dn = Vector3f::dot(incoming, normal);
    float coff = dn > 0 ? -1 : 1;
    auto delta = 1 - index_n * index_n * (1 - dn * dn) / (index_nt * index_nt);
    if (delta < 0)
        return false;
    transmitted = index_n * (incoming - dn * normal) / index_nt - coff * normal * sqrt(delta);
    return true;
}

class RayTracer
{
public:
    RayTracer() = delete;

    RayTracer(SceneParser *scene, int max_bounces, bool shadow,
              bool refract) : scene(scene), max_bounces(max_bounces),
                              use_shadow(shadow), use_refract(refract)
    {
    }

    ~RayTracer() = default;

    Vector3f traceRay(const Ray &ray, float t_min, int bounces, Hit &hit, float currentIndex = 1.f, bool debug = false) const
    {
        hit = Hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        Vector3f finalColor = Vector3f::ZERO;
        const auto group = scene->getGroup();

        bool intersected = group->intersect(ray, hit, t_min);
        auto p = ray.pointAtParameter(hit.getT());
        auto material = hit.getMaterial();
        auto normal = hit.getNormal().normalized();
        auto incoming = ray.getNormalizedDirection();
        if (debug)
        {
            printf("bounces: %d, intersected: %s, p: ", bounces, intersected ? "true" : "false");
            if (intersected)
                p.print();
            printf("normal: ");
            normal.print();
            ray.print();
        }

        if (intersected)
        {
            Vector3f direction;
            for (int li = 0; li < scene->getNumLights(); li++)
            {
                Light *light = scene->getLight(li);
                Vector3f lightColor;
                float distance;
                light->getIllumination(p, direction, lightColor, distance);
                if (use_shadow)
                {
                    // judge if this intersection is in shadow
                    Hit _hit(FLT_MAX, NULL, Vector3f(0));
                    Ray _ray(p, direction.normalized() * ray.getDirectionLength());
                    if (group->intersect(_ray, _hit, EPSILON) && _hit.getT() < distance)
                    {
                        if (debug)
                            printf("bounces: %d, Meet shadow\n", bounces);
                        continue;
                    }
                }
                finalColor += material->Shade(ray, hit, direction, lightColor);
            }
            if (debug)
            {
                printf("bounces: %d, After diffuse:", bounces);
                finalColor.print();
            }

            if (bounces < max_bounces)
            {
                float reflectivity = 1.f;
                if (use_refract && material->getRefractionIndex() > 0)
                {
                    float n = currentIndex;
                    float nt = material->getRefractionIndex();
                    if (nt == n)
                        nt = 1.f;

                    if (transmittedDirection(normal, incoming, n, nt, direction))
                    {
                        Hit _hit(FLT_MAX, NULL, Vector3f(0));
                        Ray _ray(p, direction * ray.getDirectionLength());
                        auto R0 = (nt - n) * (nt - n) / (nt + n) / (nt + n);
                        auto c = 1 - fabs(Vector3f::dot(n > nt ? direction : incoming, normal));
                        auto R = R0 + (1 - R0) * c * c * c * c * c;
                        reflectivity = R;
                        if (debug)
                        {
                            printf("tag: %lx, bounces: %d, R: %f, transmitted direction: ", &c, bounces, R);
                            direction.print();
                        }
                        finalColor += (1 - R) * traceRay(_ray, EPSILON, bounces + 1, _hit, nt, debug);
                        if (debug)
                        {
                            printf("tag: %lx, bounces: %d, after transmitted:", &c, bounces);
                            finalColor.print();
                        }
                    }
                }
                if (material->getSpecularColor() != Vector3f::ZERO)
                {
                    direction = mirrorDirection(normal, incoming);
                    Hit _hit(FLT_MAX, NULL, Vector3f(0));
                    Ray _ray(p, direction * ray.getDirectionLength());
                    if (debug)
                    {
                        printf("bounces: %d, reflect direction: ", bounces);
                        direction.print();
                    }
                    finalColor += reflectivity * material->getSpecularColor() * traceRay(_ray, EPSILON, bounces + 1, _hit, currentIndex, debug);
                    if (debug)
                    {
                        printf("bounces: %d, after reflect:", bounces);
                        finalColor.print();
                    }
                }
            }
        }
        else
        {
            finalColor = scene->getBackgroundColor(ray.getDirection());
        }
        return VectorUtils::clamp(finalColor);
    }

private:
    SceneParser *scene;
    int max_bounces;
    bool use_shadow;
    bool use_refract;
};

#endif // RAY_TRACER_H
