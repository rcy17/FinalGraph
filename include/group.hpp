#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>

// DONE: Implement Group - add data structure to store a list of Object*
class Group : public Object3D
{

public:
    Group() : objects(0)
    {
    }

    explicit Group(int num_objects) : objects(num_objects)
    {
    }

    ~Group() override
    {
    }

    bool intersect(const Ray &r, Hit &h, float t_min) override
    {
        bool intersected = false;
        for (auto object_p : objects)
        {
            intersected = object_p->intersect(r, h, t_min);
        }
        return intersected;
    }

    void addObject(int index, Object3D *obj)
    {
        objects.insert(objects.begin() + index, obj);
    }

    int getGroupSize()
    {
        return objects.size();
    }

private:
    std::vector<Object3D *> objects;
};

#endif
