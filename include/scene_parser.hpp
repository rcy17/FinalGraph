/*
* This file is merged from MIT Open Course 6-837 assignment Ray Casting
*/

#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <cassert>
#include <vecmath.h>

#include "camera.hpp"
#include "cube_map.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object3d.hpp"
#include "group.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "transform.hpp"
#include "mesh.hpp"
#include "noise.hpp"
#include "bezier.hpp"

#define MAX_PARSER_TOKEN_LENGTH 1024

class SceneParser
{
public:
    SceneParser() = delete;
    SceneParser(const char *filename);

    ~SceneParser();

    Camera *getCamera() const
    {
        return camera;
    }

    Vector3f getBackgroundColor(Vector3f dir = 0) const
    {
        if (cubemap == 0)
        {
            return background_color;
        }
        return cubemap->operator()(dir);
    }

    Vector3f getAmbientLight() const
    {
        return ambient_light;
    }

    int getNumLights() const
    {
        return num_lights;
    }

    Light *getLight(int i) const
    {
        assert(i >= 0 && i < num_lights);
        return lights[i];
    }

    int getNumMaterials() const
    {
        return num_materials;
    }

    Material *getMaterial(int i) const
    {
        assert(i >= 0 && i < num_materials);
        return materials[i];
    }

    Group *getGroup() const
    {
        return group;
    }

private:
    void parseFile();
    void parsePerspectiveCamera();
    void parseBackground();
    void parseLights();
    Light *parsePointLight();
    Light *parseDirectionalLight();
    void parseMaterials();
    Material *parseMaterial();
    Noise *parseNoise();

    Object3D *parseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
    Group *parseGroup();
    Sphere *parseSphere();
    Plane *parsePlane();
    Triangle *parseTriangle();
    Mesh *parseTriangleMesh();
    Transform *parseTransform();
    CubeMap *parseCubeMap();
    BezierCurve2D *parseBezierCurve();
    BezierSurface *parseBezierSurface();

    int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);

    Vector3f readVector3f();
    Vector2f readVector2f();
    double readFloat();
    int readInt();

    FILE *file;
    Camera *camera;
    Vector3f background_color;
    Vector3f ambient_light;
    int num_lights;
    Light **lights;
    int num_materials;
    Material **materials;
    Material *current_material;
    Group *group;
    CubeMap *cubemap;
};

#endif // SCENE_PARSER_H
