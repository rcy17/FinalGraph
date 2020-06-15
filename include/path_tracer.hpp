#ifndef PATH_TRACER_H
#define PATH_TRACER_H

#include "tracer.hpp"

class PathTracer : public Tracer
{
public:
    PathTracer() = delete;

    PathTracer(SceneParser *scene, int max_bounces, bool shadow,
               bool refract) : Tracer(scene, max_bounces, shadow, refract)
    {
    }

    ~PathTracer() = default;

    Vector3f traceRay(const Ray &ray, double t_min, int bounces, unsigned short *seed, double currentIndex = 1.f, bool debug = false) const
    {

        Hit hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        const auto group = scene->getGroup();

        if (bounces < max_bounces && group->intersect(ray, hit, t_min))
        {
            auto p = ray.pointAtParameter(hit.getT());
            auto material = hit.getMaterial();
            auto normal = hit.getNormal().normalized();
            auto incoming = ray.getNormalizedDirection();
            Vector3f color;
            Vector3f direction;
            double eta = material->getRefractionIndex();
            switch (material->getType())
            {
            case ILLUMINANT:
                color = material->getLightColor();
                break;
            case DIFFUSE:
                direction = hit.getRandomReflect(incoming, seed);
                color = material->getDiffuseColor() * traceRay({p, direction}, EPSILON, bounces + 1, seed);
                break;
            case SPECULAR:
                direction = mirrorDirection(normal, incoming);
                color = material->getSpecularColor() * traceRay({p, direction}, EPSILON, bounces + 1, seed);
                break;
            case REFRACTIVE:
            {
                Vector3f result;
                // Regard nt as red color's nt, calculate green and blue's
                // Wave length: red 700 nm, green 546 nm, blue 436 nm
                double k[3] = {1, 700. / 546, 700. / 436};
                auto c = material->getSpecularColor();
                auto reflect_direction = mirrorDirection(normal, incoming);
                for (int i = 0; i < 3; i++)
                {
                    double eta = material->getRefractionIndex();
                    auto sc = Vector3f::ZERO;
                    sc[i] = c[i];
                    assert(eta > 1);
                    eta *= k[i];
                    if (Vector3f::dot(normal, incoming) < 0)
                        eta = 1 / eta;
                    if (!transmittedDirection(normal, incoming, eta, 1, direction))
                        color += sc * traceRay({p, reflect_direction}, EPSILON, bounces + 1, seed);
                    else
                    {
                        auto R0 = (eta - 1) * (eta - 1) / (eta + 1) / (eta + 1);
                        auto c = 1 - fabs(Vector3f::dot(eta > 1 ? direction : incoming, normal));
                        auto R = R0 + (1 - R0) * c * c * c * c * c;
                        color += ((1 - R) * traceRay({p, direction}, EPSILON, bounces + 1, seed) +
                                  R * traceRay({p, reflect_direction}, EPSILON, bounces + 1, seed)) *
                                 sc;
                    }
                }

                break;
            }
            }
            return color * material->colorCorrect(ray, hit);
        }
        else
            return scene->getBackgroundColor(ray.getDirection());
    }

private:
    bool randomStop(Vector3f color)
    {
    }
};

#endif // RAY_TRACER_H
