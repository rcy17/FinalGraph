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

    Vector3f traceRay(const Ray &ray, double t_min, int bounces, unsigned short *seed, Channel channel = ALL, bool debug = false) const
    {

        Hit hit(FLT_MAX, NULL, Vector3f(0, 0, 0));
        const auto group = scene->getGroup();
        const Vector3f CHANNEL_COLOR[] = {
            Vector3f{1, 0, 0},
            Vector3f{0, 1, 0},
            Vector3f{0, 0, 1},
        };

        if (!group->intersect(ray, hit, t_min))
            return scene->getBackgroundColor(ray.getDirection());

        auto p = ray.pointAtParameter(hit.getT());
        auto material = hit.getMaterial();
        auto normal = hit.getNormal().normalized();
        auto incoming = ray.getNormalizedDirection();
        Vector3f color = material->getColor(hit, p, incoming, seed);
        if (channel != ALL)
            color = color * CHANNEL_COLOR[channel];
        Vector3f dir_reflect = mirrorDirection(normal, incoming);
        Vector3f dir_refract;
        double eta = material->getRefractionIndex();
        if (material->getType() == ILLUMINANT)
            return color;
        double possibility = std::max(std::max(color[0], color[1]), color[2]);
        if (possibility < EPS)
            return scene->getBackgroundColor();
        if (bounces >= max_bounces)
        {
            if (bounces <= 1000 && erand48(seed) < possibility)
                color = color / possibility;
            else
                return scene->getBackgroundColor();
        }

        switch (material->getType())
        {
        case DIFFUSE:
            return color * traceRay({p, hit.getRandomReflect(incoming, seed)}, EPSILON, bounces + 1, seed, channel);
        case SPECULAR:
            return color * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed, channel);
        case REFRACTIVE:
        {
            // Regard nt as red color's nt, calculate green and blue's
            // Wave length: red 700 nm, green 546 nm, blue 436 nm
            // Related to Na yellow
            double k[3] = {589. / 700, 589. / 546, 589. / 436};
            //double k[3] = {1, 1, 1};
            auto reflect_direction = mirrorDirection(normal, incoming);
            auto result = Vector3f::ZERO;
            bool go_in = Vector3f::dot(normal, incoming) < 0;
            int start = 0, stop = 3;
            if (channel != ALL)
                start = channel, stop = channel + 1;
            for (int i = start; i < stop; i++)
            {
                double eta = material->getRefractionIndex() * k[i];
                auto sc = color * CHANNEL_COLOR[i];
                if (go_in)
                    eta = 1 / eta;
                if (!transmittedDirection(normal, incoming, eta, 1, dir_refract))
                    result += sc * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed, Channel(i));
                else
                {
                    auto R0 = (eta - 1) * (eta - 1) / (eta + 1) / (eta + 1);
                    auto c = 1 - fabs(Vector3f::dot(eta > 1 ? dir_refract : incoming, normal));
                    auto R = R0 + (1 - R0) * c * c * c * c * c;
                    possibility = 0.25 + R / 2;
                    if (bounces)
                        // Mento Carlo by Russian roulette
                        if (erand48(seed) < possibility)
                            result += sc * R / possibility * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed, Channel(i));
                        else
                            result += sc * (1 - R) / (1 - possibility) * traceRay({p, dir_refract}, EPSILON, bounces + 1, seed, Channel(i));
                    else
                        // trace two direction for the first time
                        result += sc * ((1 - R) * traceRay({p, dir_refract}, EPSILON, bounces + 1, seed, Channel(i)) +
                                           R * traceRay({p, dir_reflect}, EPSILON, bounces + 1, seed, Channel(i)));
                }
            }
            return result;
        }
        default:
            assert(0);
        }
    }

private:
};

#endif // RAY_TRACER_H
