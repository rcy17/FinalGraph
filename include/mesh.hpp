#ifndef MESH_H
#define MESH_H
#include <vector>
#include "object3d.hpp"
#include "octree.hpp"
#include "triangle.hpp"
#include <vecmath.h>
//by default counterclockwise winding is front face
struct Trig
{
    Trig()
    {
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;
    }
    int &operator[](int i) { return x[i]; }
    int operator[](int i) const { return x[i]; }
    int x[3];
    int texID[3];
};

class Mesh : public Object3D
{
public:
    Mesh(const char *filename, Material *m);
    std::vector<Vector3f> v;
    std::vector<Trig> t;
    std::vector<Vector3f> n;
    std::vector<Vector2f> texCoord;

    virtual bool intersect(const Ray &r, Hit &h, float t_min) override;
    bool intersectTrig(int idx, const Ray &r, Hit &h, float t_min);

private:
    void computeNorm();
    Octree octree;
};

#endif
