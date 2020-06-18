#ifndef DISPERSE_TRACER_H
#define RAY_TRADISPERSE_TRACER_HCER_H

#include "tracer.hpp"
#include "color.hpp"

#ifndef EPSILON
#define EPSILON 0.00001f
#endif

class DisperseTracer : public Tracer
{
public:
    DisperseTracer() = delete;

    DisperseTracer(SceneParser *scene, int max_bounces) : Tracer(scene, max_bounces, true, true)
    {
    }

    ~DisperseTracer() = default;

    Vector3f traceRay(const Ray &ray, double t_min, int bounces, unsigned short *seed, double currentIndex = 1.f, bool debug = false) const
    {
        if (currentIndex < 100.)
        {
            Vector3f color;
            const int total = 36;
            for (int i = 0; i < total; i++)
            {
                double len = 380 + i * 10;
                color += traceRay(ray, t_min, bounces, seed, len, debug) / total;
            }
            return color;
        }
        Hit hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        Vector3f finalColor = Vector3f::ZERO;
        const auto group = scene->getGroup();

        bool intersected = group->intersect(ray, hit, t_min);
        auto p = ray.pointAtParameter(hit.getT());
        auto material = hit.getMaterial();
        auto normal = hit.getNormal().normalized();
        auto incoming = ray.getNormalizedDirection();

        if (intersected)
        {
            Vector3f direction;
            for (int li = 0; li < scene->getNumLights(); li++)
            {
                Light *light = scene->getLight(li);
                Vector3f lightColor;
                double distance = currentIndex;
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

            if (bounces < max_bounces)
            {
                double reflectivity = 1.f;
                if (use_refract && material->getRefractionIndex() > 0)
                {
                    double n = 1;
                    double nt = material->getRefractionIndex();
                    nt = nt / 589.3 * currentIndex;
                    if (Vector3f::dot(incoming, normal) > 0)
                        swap(n, nt);

                    if (transmittedDirection(normal, incoming, n, nt, direction))
                    {
                        Ray _ray(p, direction * ray.getDirectionLength());
                        auto R0 = (nt - n) * (nt - n) / (nt + n) / (nt + n);
                        auto c = 1 - fabs(Vector3f::dot(n > nt ? direction : incoming, normal));
                        auto R = R0 + (1 - R0) * c * c * c * c * c;
                        reflectivity = R;
                        if (debug)
                        {
                            printf("tag: %lx, bounces: %d, R: %lf, transmitted direction: ", &c, bounces, R);
                            direction.print();
                        }
                        finalColor += (1 - R) * traceRay(_ray, EPSILON, bounces + 1, seed, nt, debug);
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
                    Ray _ray(p, direction * ray.getDirectionLength());
                    if (debug)
                    {
                        printf("bounces: %d, reflect direction: ", bounces);
                        direction.print();
                    }
                    finalColor += reflectivity * material->getSpecularColor() *
                                  traceRay(_ray, EPSILON, bounces + 1, seed, currentIndex, debug);
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
};

#endif // DISPERSE_TRACER_H
