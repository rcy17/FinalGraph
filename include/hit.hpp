#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include <float.h>

class Material;

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

    Hit(float _t, Material *m, const Vector3f &n)
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

    float getT() const
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

    void set(float _t, Material *m, const Vector3f &n)
    {
        t = _t;
        material = m;
        normal = n;
    }

    bool hasTex;
    Vector2f texCoord;

private:
    float t;
    Material *material;
    Vector3f normal;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
