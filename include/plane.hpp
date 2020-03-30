#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D
{
public:
    Plane()
    {
    }

    Plane(const Vector3f &normal, double d, Material *m) : Object3D(m), normal(normal), d(d)
    {
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double t_min) override
    {
        auto t = -(Vector3f::dot(normal, r.getOrigin()) - d) /
                 (Vector3f::dot(normal, r.getDirection()));
        if (t > h.getT() || t < t_min || t < 0)
            return false;
        h.set(t, material, Vector3f::dot(r.getDirection(), normal) < 0 ? normal : -normal);
        return true;
    }

protected:
    Vector3f normal;
    double d;
};

#endif //PLANE_H
