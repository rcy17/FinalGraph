#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D
{
public:
    Sphere() : center(0.), radius(1.), radius_square(1.)
    {
        // unit ball at the center
    }

    Sphere(const Vector3f &center,
           double radius,
           Material *material) : Object3D(material),
                                 center(center),
                                 radius(radius),
                                 radius_square(radius * radius)
    {
        //
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double t_min) const override
    {
        auto l = center - r.getOrigin();
        auto l_square = l.squaredLength();
        auto t_p = Vector3f::dot(l, r.getNormalizedDirection());

        auto d_square = l_square - t_p * t_p;
        if (d_square >= radius_square)
            return false;
        // t2 is t' in PPT
        auto t2 = sqrt(radius_square - d_square);
        auto t_close = (t_p - t2) / r.getDirectionLength();
        auto t_far = (t_p + t2) / r.getDirectionLength();
        auto t = t_close > t_min ? t_close : t_far;
        if (t > h.getT() || t < t_min)
            return false;
        auto p = r.pointAtParameter(t);
        const auto normal = (p - center).normalized();
        h.set(t, material, normal);
        return true;
    }

protected:
    Vector3f center;
    double radius;
    double radius_square;
};

#endif
