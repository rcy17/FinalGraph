#ifndef PATH_TRACER_H
#define PATH_TRACER_H

#include "tracer.hpp"

class PathTracer : public Tracer
{
public:
    PathTracer() = delete;

    PathTracer(SceneParser *scene, int max_bounces) : Tracer(scene, max_bounces, true, true)
    {
    }

    ~PathTracer() = default;

    Vector3f traceRay(const Ray &ray, double t_min, int bounces, unsigned short *seed, Channel channel = ALL, bool debug = false) const
    {

        Hit hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        const auto group = scene->getGroup();

        if (!group->intersect(ray, hit, t_min))
            return scene->getBackgroundColor(ray.getDirection());

        auto p = ray.pointAtParameter(hit.getT());
        auto material = hit.getMaterial();
        auto normal = hit.getNormal().normalized();
        auto incoming = ray.getNormalizedDirection();
        Vector3f color = material->getColor(hit, p);
        Vector3f dir_reflect = mirrorDirection(normal, incoming);
        Vector3f dir_refract;
        double eta = material->getRefractionIndex();
        if (material->getType() == ILLUMINANT)
            return color;
        double possibility = std::max(std::max(color[0], color[1]), color[2]);
        if (bounces >= max_bounces)
        {
            if (erand48(seed) < possibility)
                color = color / possibility;
            else
                return scene->getBackgroundColor();
        }

        switch (material->getType())
        {
        case DIFFUSE:
            return color * traceRay({p, hit.getRandomReflect(incoming, seed)}, EPSILON, bounces + 1, seed);
        case SPECULAR:
            return color * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed);
        case REFRACTIVE:
            assert(eta > 1);
            if (Vector3f::dot(normal, incoming) < 0)
                eta = 1 / eta;
            if (!transmittedDirection(normal, incoming, eta, 1, dir_refract))
                // total relection
                return color * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed);
            else
            {
                auto R0 = (eta - 1) * (eta - 1) / (eta + 1) / (eta + 1);
                auto c = 1 - fabs(Vector3f::dot(eta > 1 ? dir_refract : incoming, normal));
                auto R = R0 + (1 - R0) * c * c * c * c * c;
                possibility = 0.25 + R / 2;
                if (bounces)
                    // Mento Carlo by Russian roulette
                    if (erand48(seed) < possibility)
                        return color * R / possibility * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed);
                    else
                        return color * (1 - R) / (1 - possibility) * traceRay({p, dir_refract}, EPSILON, bounces + 1, seed);
                else
                    // trace two direction for the first time
                    return color * ((1 - R) * traceRay({p, dir_refract}, EPSILON, bounces + 1, seed) +
                                    R * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed));
            }
        default:
            assert(0);
        }
    }

private:
};

#endif // RAY_TRACER_H
