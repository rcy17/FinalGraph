#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// DONE: Implement functions and add more fields as necessary

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

    bool intersect(const Ray &r, Hit &h, double t_min) override
    {
        const auto &origin = r.getOrigin();
        const auto &direction = r.getDirection().normalized();
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
        auto t = t_p + t2 * (relation == INSIDE ? 1 : -1);
        t /= r.getDirection().length();
        if (t < t_min || t > h.getT())
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

    Relation getRelation(double square_length)
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
    double radius;
    double radius_square;
};

#endif
