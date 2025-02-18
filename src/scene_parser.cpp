#include <cstdio>
#include <cstring>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>

#include "scene_parser.hpp"

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

SceneParser::SceneParser(const char *filename)
{

    // initialize some reasonable default values
    group = NULL;
    camera = NULL;
    background_color = Vector3f(0.5, 0.5, 0.5);
    ambient_light = Vector3f(0, 0, 0);
    num_lights = 0;
    lights = NULL;
    num_materials = 0;
    materials = NULL;
    current_material = NULL;
    cubemap = 0;
    // parse the file
    if (!filename)
    {
        return;
    }

    const char *ext = &filename[strlen(filename) - 4];

    if (strcmp(ext, ".txt") != 0)
    {
        printf("wrong file name extension\n");
        exit(0);
    }
    file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("cannot open scene file\n");
        exit(0);
    }
    parseFile();
    fclose(file);
    file = NULL;

    // if no lights are specified, set ambient light to white
    // (do solid color ray casting)
    if (num_lights == 0)
    {
        printf("WARNING:    No lights specified\n");
        ambient_light = Vector3f(1, 1, 1);
    }
}

SceneParser::~SceneParser()
{
    if (group != NULL)
        delete group;
    if (camera != NULL)
        delete camera;
    int i;
    for (i = 0; i < num_materials; i++)
    {
        delete materials[i];
    }
    delete[] materials;
    for (i = 0; i < num_lights; i++)
    {
        delete lights[i];
    }
    delete[] lights;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile()
{
    //
    // at the top level, the scene can have a camera,
    // background color and a group of objects
    // (we add lights and other things in future assignments)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    while (getToken(token))
    {
        if (!strcmp(token, "PerspectiveCamera"))
        {
            parsePerspectiveCamera();
        }
        else if (!strcmp(token, "Background"))
        {
            parseBackground();
        }
        else if (!strcmp(token, "Lights"))
        {
            parseLights();
        }
        else if (!strcmp(token, "Materials"))
        {
            parseMaterials();
        }
        else if (!strcmp(token, "Group"))
        {
            group = parseGroup();
        }
        else
        {
            printf("Unknown token in parseFile: '%s'\n", token);
            exit(0);
        }
    }
}

// ====================================================================
// ====================================================================

void SceneParser::parsePerspectiveCamera()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the camera parameters
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token);
    assert(!strcmp(token, "direction"));
    Vector3f direction = readVector3f();
    getToken(token);
    assert(!strcmp(token, "up"));
    Vector3f up = readVector3f();
    getToken(token);
    assert(!strcmp(token, "angle"));
    double angle_degrees = readFloat();
    double angle_radians = DegreesToRadians(angle_degrees);
    // Here are some options
    getToken(token);
    int width = 0, height = 0;
    double distance = 0;
    bool use_depth = false;
    double aperture = 0;
    while (strcmp(token, "}"))
    {
        if (!strcmp(token, "width"))
        {
            width = readInt();
        }
        else if (!strcmp(token, "height"))
        {
            height = readInt();
        }
        else if (!strcmp(token, "distance"))
        {
            distance = readFloat();
        }
        else if (!strcmp(token, "aperture"))
        {
            aperture = readFloat();
            use_depth = true;
        }
        else
        {
            printf("Ignore token %s\n", token);
        }
        getToken(token);
    }
    if (use_depth)
        camera = new DepthCamera(center, direction, up, width, height, angle_radians, distance, aperture);
    else
        camera = new PerspectiveCamera(center, direction, up, width, height, angle_radians, distance);
}

void SceneParser::parseBackground()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    getToken(token);
    assert(!strcmp(token, "{"));
    while (1)
    {
        getToken(token);
        if (!strcmp(token, "}"))
        {
            break;
        }
        else if (!strcmp(token, "color"))
        {
            background_color = readVector3f();
        }
        else if (!strcmp(token, "ambientLight"))
        {
            ambient_light = readVector3f();
        }
        else if (strcmp(token, "cubeMap") == 0)
        {
            cubemap = parseCubeMap();
        }
        else
        {
            printf("Unknown token in parseBackground: '%s'\n", token);
            assert(0);
        }
    }
}

CubeMap *SceneParser::parseCubeMap()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    return new CubeMap(token);
}
// ====================================================================
// ====================================================================

void SceneParser::parseLights()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    // read in the number of objects
    getToken(token);
    assert(!strcmp(token, "numLights"));
    num_lights = readInt();
    lights = new Light *[num_lights];
    // read in the objects
    int count = 0;
    while (num_lights > count)
    {
        getToken(token);
        if (!strcmp(token, "DirectionalLight"))
        {
            lights[count] = parseDirectionalLight();
        }
        else if (strcmp(token, "PointLight") == 0)
        {
            lights[count] = parsePointLight();
        }
        else
        {
            printf("Unknown token in parseLight: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    getToken(token);
    assert(!strcmp(token, "}"));
}

Light *SceneParser::parseDirectionalLight()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "direction"));
    Vector3f direction = readVector3f();
    getToken(token);
    assert(!strcmp(token, "color"));
    Vector3f color = readVector3f();
    getToken(token);
    assert(!strcmp(token, "}"));
    return new DirectionalLight(direction, color);
}
Light *SceneParser::parsePointLight()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Vector3f position, color;
    double falloff = 0;
    getToken(token);
    assert(!strcmp(token, "{"));
    while (1)
    {
        getToken(token);
        if (strcmp(token, "position") == 0)
        {
            position = readVector3f();
        }
        else if (strcmp(token, "color") == 0)
        {
            color = readVector3f();
        }
        else if (strcmp(token, "falloff") == 0)
        {
            falloff = readFloat();
        }
        else
        {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    return new PointLight(position, color, falloff);
}
// ====================================================================
// ====================================================================

void SceneParser::parseMaterials()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    // read in the number of objects
    getToken(token);
    assert(!strcmp(token, "numMaterials"));
    num_materials = readInt();
    materials = new Material *[num_materials];
    // read in the objects
    int count = 0;
    while (num_materials > count)
    {
        getToken(token);
        if (!strcmp(token, "Material") ||
            !strcmp(token, "PhongMaterial"))
        {
            materials[count] = parseMaterial();
        }
        else
        {
            printf("Unknown token in parseMaterial: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    getToken(token);
    assert(!strcmp(token, "}"));
}

Material *SceneParser::parseMaterial()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Vector3f diffuseColor(0, 0, 0), specularColor(0, 0, 0), lightColor(0, 0, 0);
    double shininess = 0;
    double refractionIndex = 0;
    getToken(token);
    assert(!strcmp(token, "{"));
    Noise *noise = 0;
    while (1)
    {
        getToken(token);
        if (strcmp(token, "diffuseColor") == 0)
        {
            diffuseColor = readVector3f();
        }
        else if (strcmp(token, "specularColor") == 0)
        {
            specularColor = readVector3f();
        }
        else if (strcmp(token, "lightColor") == 0)
        {
            lightColor = readVector3f();
        }
        else if (strcmp(token, "shininess") == 0)
        {
            shininess = readFloat();
        }
        else if (strcmp(token, "refractionIndex") == 0)
        {
            refractionIndex = readFloat();
        }
        else if (strcmp(token, "texture") == 0)
        {
            getToken(filename);
        }
        // TODO:unimplemented
        else if (strcmp(token, "bump") == 0)
        {
            getToken(token);
        }
        else if (strcmp(token, "Noise") == 0)
        {
            noise = parseNoise();
        }
        else
        {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    Material *answer = new Material(diffuseColor, specularColor, lightColor, refractionIndex);
    if (filename[0] != 0)
    {
        answer->loadTexture(filename);
    }
    if (noise != 0)
    {
        answer->setNoise(*noise);
        delete noise;
    }
    return answer;
}

Noise *SceneParser::parseNoise()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Vector3f color[2];
    int colorIdx = 0;
    int octaves = 0;
    double frequency = 1;
    double amplitude = 1;
    getToken(token);
    assert(!strcmp(token, "{"));
    while (1)
    {
        getToken(token);
        if (strcmp(token, "color") == 0)
        {
            if (colorIdx > 1)
            {
                printf("Error parsing noise\n");
            }
            else
            {
                color[colorIdx] = readVector3f();
                colorIdx++;
            }
        }
        else if (strcmp(token, "octaves") == 0)
        {
            octaves = readInt();
        }
        else if (strcmp(token, "frequency") == 0)
        {
            frequency = readFloat();
        }
        else if (strcmp(token, "amplitude") == 0)
        {
            amplitude = readFloat();
        }
        else
        {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    return new Noise(octaves, color[0], color[1], frequency, amplitude);
}
// ====================================================================
// ====================================================================

Object3D *SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH])
{
    Object3D *answer = NULL;
    if (!strcmp(token, "Group"))
    {
        answer = (Object3D *)parseGroup();
    }
    else if (!strcmp(token, "Sphere"))
    {
        answer = (Object3D *)parseSphere();
    }
    else if (!strcmp(token, "Plane"))
    {
        answer = (Object3D *)parsePlane();
    }
    else if (!strcmp(token, "Triangle"))
    {
        answer = (Object3D *)parseTriangle();
    }
    else if (!strcmp(token, "TriangleMesh"))
    {
        answer = (Object3D *)parseTriangleMesh();
    }
    else if (!strcmp(token, "Transform"))
    {
        answer = (Object3D *)parseTransform();
    }
    else if (!strcmp(token, "BezierCurve"))
    {
        answer = (Object3D *)parseBezierCurve();
    }
    else if (!strcmp(token, "RevSurface"))
    {
        answer = (Object3D *)parseBezierSurface();
    }
    else
    {
        printf("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
    return answer;
}

// ====================================================================
// ====================================================================

Group *SceneParser::parseGroup()
{
    //
    // each group starts with an integer that specifies
    // the number of objects in the group
    //
    // the material index sets the material of all objects which follow,
    // until the next material index (scoping for the materials is very
    // simple, and essentially ignores any tree hierarchy)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));

    // read in the number of objects
    getToken(token);
    assert(!strcmp(token, "numObjects"));
    int num_objects = readInt();

    Group *answer = new Group(num_objects);

    // read in the objects
    int count = 0;
    while (num_objects > count)
    {
        getToken(token);
        if (!strcmp(token, "MaterialIndex"))
        {
            // change the current material
            int index = readInt();
            assert(index >= 0 && index <= getNumMaterials());
            current_material = getMaterial(index);
        }
        else
        {
            Object3D *object = parseObject(token);
            assert(object != NULL);
            answer->addObject(count, object);

            count++;
        }
    }
    getToken(token);
    assert(!strcmp(token, "}"));

    // return the group
    return answer;
}

// ====================================================================
// ====================================================================

Sphere *SceneParser::parseSphere()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token);
    assert(!strcmp(token, "radius"));
    double radius = readFloat();
    getToken(token);
    assert(!strcmp(token, "}"));
    assert(current_material != NULL);
    return new Sphere(center, radius, current_material);
}

Plane *SceneParser::parsePlane()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "normal"));
    Vector3f normal = readVector3f();
    getToken(token);
    assert(!strcmp(token, "offset"));
    double offset = readFloat();
    getToken(token);
    double scale = 1.0;
    if (!strcmp(token, "scale"))
    {
        scale = readFloat();
        getToken(token);
    }
    Vector3f center = normal * offset;
    if (!strcmp(token, "center"))
    {
        center = readVector3f();
        getToken(token);
    }
    assert(!strcmp(token, "}"));
    assert(current_material != NULL);
    return new Plane(normal, offset, current_material, center, scale);
}

Triangle *SceneParser::parseTriangle()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "vertex0"));
    Vector3f v0 = readVector3f();
    getToken(token);
    assert(!strcmp(token, "vertex1"));
    Vector3f v1 = readVector3f();
    getToken(token);
    assert(!strcmp(token, "vertex2"));
    Vector3f v2 = readVector3f();
    getToken(token);
    assert(!strcmp(token, "}"));
    assert(current_material != NULL);
    return new Triangle(v0, v1, v2, current_material);
}

Mesh *SceneParser::parseTriangleMesh()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    // get the filename
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "obj_file"));
    getToken(filename);
    getToken(token);
    assert(!strcmp(token, "}"));
    const char *ext = &filename[strlen(filename) - 4];
    assert(!strcmp(ext, ".obj"));
    Mesh *answer = new Mesh(filename, current_material);

    return answer;
}

Transform *SceneParser::parseTransform()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Matrix4f matrix = Matrix4f::identity();
    Object3D *object = NULL;
    getToken(token);
    assert(!strcmp(token, "{"));
    // read in transformations:
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    getToken(token);

    while (1)
    {
        if (!strcmp(token, "Scale"))
        {
            Vector3f s = readVector3f();
            matrix = matrix * Matrix4f::scaling(s[0], s[1], s[2]);
        }
        else if (!strcmp(token, "UniformScale"))
        {
            double s = readFloat();
            matrix = matrix * Matrix4f::uniformScaling(s);
        }
        else if (!strcmp(token, "Translate"))
        {
            matrix = matrix * Matrix4f::translation(readVector3f());
        }
        else if (!strcmp(token, "XRotate"))
        {
            matrix = matrix * Matrix4f::rotateX(DegreesToRadians(readFloat()));
        }
        else if (!strcmp(token, "YRotate"))
        {
            matrix = matrix * Matrix4f::rotateY(DegreesToRadians(readFloat()));
        }
        else if (!strcmp(token, "ZRotate"))
        {
            matrix = matrix * Matrix4f::rotateZ(DegreesToRadians(readFloat()));
        }
        else if (!strcmp(token, "Rotate"))
        {
            getToken(token);
            assert(!strcmp(token, "{"));
            Vector3f axis = readVector3f();
            double degrees = readFloat();
            double radians = DegreesToRadians(degrees);
            matrix = matrix * Matrix4f::rotation(axis, radians);
            getToken(token);
            assert(!strcmp(token, "}"));
        }
        else if (!strcmp(token, "Matrix4f"))
        {
            Matrix4f matrix2 = Matrix4f::identity();
            getToken(token);
            assert(!strcmp(token, "{"));
            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    double v = readFloat();
                    matrix2(i, j) = v;
                }
            }
            getToken(token);
            assert(!strcmp(token, "}"));
            matrix = matrix2 * matrix;
        }
        else
        {
            // otherwise this must be an object,
            // and there are no more transformations
            object = parseObject(token);
            break;
        }
        getToken(token);
    }

    assert(object != NULL);
    getToken(token);
    assert(!strcmp(token, "}"));
    return new Transform(matrix, object);
}

// ====================================================================
// ====================================================================

BezierCurve2D *SceneParser::parseBezierCurve()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "controls"));
    vector<Vector3f> controls;
    while (true)
    {
        getToken(token);
        if (!strcmp(token, "["))
        {
            controls.push_back(readVector3f());
            getToken(token);
            assert(!strcmp(token, "]"));
        }
        else if (!strcmp(token, "}"))
        {
            break;
        }
        else
        {
            printf("Incorrect format for BezierCurve!\n");
            exit(0);
        }
    }
    Vector2f *controls2d = new Vector2f[controls.size()];
    for (size_t i = 0; i < controls.size(); ++i)
        controls2d[i] = Vector2f(controls[i].x(), controls[i].y());
    BezierCurve2D *answer = new BezierCurve2D(controls2d, controls.size(), controls.size());
    delete[] controls2d;

    return answer;
}

// ====================================================================
// ====================================================================

BezierSurface *SceneParser::parseBezierSurface()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert(!strcmp(token, "{"));
    getToken(token);
    assert(!strcmp(token, "profile"));
    BezierCurve2D *profile;
    getToken(token);
    if (!strcmp(token, "BezierCurve"))
    {
        profile = parseBezierCurve();
    }
    else
    {
        printf("Unknown profile type in parseBezierSurface: '%s'\n", token);
        exit(0);
    }
    getToken(token);
    assert(!strcmp(token, "}"));
    auto *answer = new BezierSurface(profile, current_material);
    return answer;
}

// ====================================================================
// ====================================================================

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH])
{
    // for simplicity, tokens must be separated by whitespace
    assert(file != NULL);
    int success = fscanf(file, "%s ", token);
    if (success == EOF)
    {
        token[0] = '\0';
        return 0;
    }
    return 1;
}

Vector3f SceneParser::readVector3f()
{
    double x, y, z;
    int count = fscanf(file, "%lf %lf %lf", &x, &y, &z);
    if (count != 3)
    {
        printf("Error trying to read 3 doubles to make a Vector3f\n");
        assert(0);
    }
    return Vector3f(x, y, z);
}

Vector2f SceneParser::readVector2f()
{
    double u, v;
    int count = fscanf(file, "%lf %lf", &u, &v);
    if (count != 2)
    {
        printf("Error trying to read 2 doubles to make a Vec2f\n");
        assert(0);
    }
    return Vector2f(u, v);
}

double SceneParser::readFloat()
{
    double answer;
    int count = fscanf(file, "%lf", &answer);
    if (count != 1)
    {
        printf("Error trying to read 1 double\n");
        assert(0);
    }
    return answer;
}

int SceneParser::readInt()
{
    int answer;
    int count = fscanf(file, "%d", &answer);
    if (count != 1)
    {
        printf("Error trying to read 1 int\n");
        assert(0);
    }
    return answer;
}
