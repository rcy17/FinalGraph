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
           float radius,
           Material *material) : Object3D(material),
                                 center(center),
                                 radius(radius),
                                 radius_square(radius * radius)
    {
        //
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float t_min) override
    {
        const auto &origin = r.getOrigin();
        const auto &direction = r.getNormalizedDirection();
        auto l = center - origin;
        auto l_square = l.squaredLength();
        auto relation = getRelation(l_square);
        if (relation == JUSTON)
            return false;
        auto t_p = Vector3f::dot(l, direction);

        if (t_p < 0 && relation == OUTSIDE)
            return false;
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
        h.set(t, material, relation == OUTSIDE ? normal : -normal);
        return true;
    }

protected:
    enum Relation
    {
        INSIDE,
        JUSTON,
        OUTSIDE,
    };

    Relation getRelation(float square_length)
    {
        if (square_length > radius)
        {
            return OUTSIDE;
        }
        else if (square_length < radius)
        {
            return INSIDE;
        }
        else
        {
            return JUSTON;
        }
    }

    Vector3f center;
    float radius;
    float radius_square;
};

#endif
