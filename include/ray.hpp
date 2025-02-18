#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <iostream>
#include <Vector3f.h>

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray
{
public:
    Ray() = delete;
    Ray(const Vector3f &orig, const Vector3f &dir)
    {
        origin = orig;
        direction = dir;
        direction_length = dir.length();
        normalized_direction = dir / direction_length;
    }

    Ray(const Ray &r)
    {
        origin = r.origin;
        direction = r.direction;
        normalized_direction = r.normalized_direction;
        direction_length = r.direction_length;
    }

    const Vector3f &getOrigin() const
    {
        return origin;
    }

    const Vector3f &getDirection() const
    {
        return direction;
    }

    const Vector3f &getNormalizedDirection() const
    {
        return normalized_direction;
    }

    double getDirectionLength() const
    {
        return direction_length;
    }

    Vector3f pointAtParameter(double t) const
    {
        return origin + direction * t;
    }

    void print() const
    {
        printf("Ray {\n");
        origin.print();
        direction.print();
        printf("}\n");
    }

private:
    Vector3f origin;
    Vector3f direction;
    Vector3f normalized_direction;
    double direction_length;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r)
{
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif // RAY_H
