#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D
{
public:
    Plane()
    {
    }

    Plane(const Vector3f &normal, double d, Material *m, const Vector3f &center,
          double scale = 1.) : Object3D(m), normal(normal), d(d), scale(scale), center(center)
    {
        // Some dirty work to deal with plane's texture
        if (abs(normal[0]) < EPS)
        {
            base[0] = Vector3f(1, 0, 0);
            hasTex = true;
        }
        else if (abs(normal[1]) < EPS)
        {
            base[0] = Vector3f(0, 1, 0);
            hasTex = true;
        }
        else if (abs(normal[2]) < EPS)
        {
            base[0] = Vector3f(0, 0, 1);
            hasTex = true;
        }
        if (hasTex)
        {
            base[1] = Vector3f::cross(base[0], normal);
        }
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double t_min) const override
    {
        auto t = -(Vector3f::dot(normal, r.getOrigin()) - d) /
                 (Vector3f::dot(normal, r.getDirection()));
        if (t > h.getT() || t < t_min || t < 0)
            return false;
        h.set(t, material, normal);
        if (normal[0] < -0.5)
        {
            int a = 2;
            a++;
        }
        if (hasTex)
        {
            h.hasTex = true;
            h.texCoord = getTexCoord(r.pointAtParameter(t) - center);
        }
        return true;
    }

protected:
    Vector3f normal;
    double d;
    Vector3f center;
    bool hasTex;
    Vector3f base[2];
    double scale;

private:
    Vector2f getTexCoord(const Vector3f &p) const
    {
        double u = Vector3f::dot(p, base[0]) / scale;
        u -= floor(u);
        double v = Vector3f::dot(p, base[1]) / scale;
        v -= floor(v);
        return {u, v};
    }
};

#endif //PLANE_H
