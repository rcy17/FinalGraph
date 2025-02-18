#ifndef VEC_UTILS_H
#define VEC_UTILS_H

#include <vecmath.h>

class VectorUtils
{
public:
    static Vector3f min(const Vector3f &b, const Vector3f &c)
    {
        Vector3f out;

        for (int i = 0; i < 3; ++i)
        {
            out[i] = (b[i] < c[i]) ? b[i] : c[i];
        }

        return out;
    }

    static Vector3f max(const Vector3f &b, const Vector3f &c)
    {
        Vector3f out;

        for (int i = 0; i < 3; ++i)
        {
            out[i] = (b[i] > c[i]) ? b[i] : c[i];
        }

        return out;
    }

    static double max_element(const Vector3f &v)
    {
        return std::max(std::max(v[0], v[1]), v[2]);
    }

    static Vector3f clamp(const Vector3f &data, double low = 0, double high = 1)
    {
        Vector3f out = data;
        for (int i = 0; i < 3; ++i)
        {
            if (out[i] < low)
            {
                out[i] = low;
            }
            if (out[i] > high)
            {
                out[i] = high;
            }
        }

        return out;
    }

    // transforms a 3D point using a matrix, returning a 3D point
    static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point)
    {
        return (mat * Vector4f(point, 1)).xyz();
    }

    // transform a 3D directino using a matrix, returning a direction
    // This function *does not* take the inverse tranpose for you.
    static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir)
    {
        return (mat * Vector4f(dir, 0)).xyz();
    }
};

#endif // VEC_UTILS_H
