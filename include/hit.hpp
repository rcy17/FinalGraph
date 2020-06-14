#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include <float.h>
#include <cstdlib>
#include <cmath>

class Material;
#define EPS 1e-3

class Hit
{
public:
    // constructors
    Hit()
    {
        material = nullptr;
        t = FLT_MAX;
        hasTex = false;
    }

    Hit(double _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
        hasTex = false;
    }

    Hit(const Hit &h)
    {
        t = h.t;
        material = h.material;
        normal = h.normal;
        hasTex = h.hasTex;
    }

    // destructor
    ~Hit() = default;

    double getT() const
    {
        return t;
    }

    Material *getMaterial() const
    {
        return material;
    }

    const Vector3f &getNormal() const
    {
        return normal;
    }

    void set(double _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
    }

    const Vector3f getRandomReflect(const Vector3f &incoming, unsigned short *seed) const
    {
        Vector3f result;
        while (true)
        {
            double theta = 2 * M_PI * erand48(seed);
            double phi = M_PI * erand48(seed);
            Vector3f v(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));
            double a = Vector3f::dot(incoming, normal);
            double b = Vector3f::dot(v, normal);
            if (fabs(b) < EPS)
                continue;
            result = a * b < 0 ? v : -v;
            break;
        }
        return result;
    }

    bool hasTex;
    Vector2f texCoord;

private:
    double t;
    Material *material;
    Vector3f normal;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
