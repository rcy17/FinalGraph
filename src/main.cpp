#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "argparser.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "vector_utils.hpp"
#include "ray_tracer.hpp"
#include "path_tracer.hpp"

#include <string>

using namespace std;

Image render(const ArgParser &parser, SceneParser *scene, Camera *camera, int offset, int y_range)
{
    Image image(camera->getWidth(), y_range);
    Tracer *tracer;
    switch (parser.type)
    {
    case RT:
        tracer = new RayTracer(scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    case PT:
        tracer = new PathTracer(scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    }

#pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < y_range; y++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", y, y_range);
        for (int x = 0; x < image.Width(); x++)
        {
            int _y = y + offset;
            bool debug = false;
            if (_y == 32 && x == 17 && 0)
            {
                debug = true;
            }
            Vector3f color;
            unsigned short seed[3] = {_y, _y * x, static_cast<unsigned short>(_y * x * _y)};
            for (int i = 0; i < parser.spp; i++)
            {
                if (parser.jitter)
                {
                    auto dx = erand48(seed);
                    auto dy = erand48(seed);
                    Ray camRay = camera->generateRay(Vector2f(x - 0.5 + dx, _y - 0.5 + dy));
                    color += tracer->traceRay(camRay, EPSILON, 0, seed, 1.f, debug);
                }
                else
                {
                    Ray camRay = camera->generateRay(Vector2f(x, _y));
                    color += tracer->traceRay(camRay, EPSILON, 0, seed, 1.f, debug);
                }
            }
            image.SetPixel(x, y, VectorUtils::clamp(color / parser.spp));
        }
    }
    delete tracer;
    fprintf(stderr, "\rprocessing %5d/%-5d\n", y_range, y_range);
    return image;
}

Image merge(const ArgParser &parser)
{
    Image image(parser.width * (1 + 2 * parser.filter), parser.height * (1 + 2 * parser.filter));
    image.Merge(parser.segments);
    return image;
}

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }
    ArgParser parser(argc, argv);

    SceneParser scene(parser.input_file);
    auto camera = scene.getCamera();
    if (parser.set_size)
        camera->setSize(parser.width, parser.height);
    if (parser.jitter)
        camera->setSize(camera->getWidth() * 3, camera->getHeight() * 3);

    int offset = parser.offset;
    int y_range = (parser.size ? parser.size : camera->getHeight() - offset);
    bool segment = y_range != camera->getHeight();

    auto image = parser.segments.empty() ? render(parser, &scene, camera, offset, y_range) : merge(parser);

    if (segment)
    {
        // In this case, just save raw data
        image.SaveRaw(parser.output_file);
    }
    else if (parser.filter)
    {
        image.GaussianBlur();
        Image result(image.Width() / 3, image.Height() / 3);
        image.DownSampling(&result);
        result.SaveImage(parser.output_file, parser.gamma);
    }
    else
    {
        image.SaveImage(parser.output_file, parser.gamma);
    }
    return 0;
}
