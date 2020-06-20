#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <string>

#include "argparser.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "vector_utils.hpp"
#include "ray_tracer.hpp"
#include "path_tracer.hpp"
#include "channel_tracer.hpp"

using namespace std;

Image render(const ArgParser &parser, SceneParser *scene, int height, int width, int offset, int y_range)
{
    auto camera = scene->getCamera();
    camera->setSize(width, height);
    Image image(camera->getWidth(), y_range);
    Tracer *tracer;
    switch (parser.type)
    {
    case RT:
        tracer = new RayTracer(scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    case PT:
        tracer = new PathTracer(scene, parser.bounces);
        break;
    case CT:
        tracer = new ChannelTracer(scene, parser.bounces);
        break;
    }

    const double t_min = camera->getDistance() + EPSILON;

#pragma omp parallel for schedule(dynamic, 1)
    for (int _y = 0; _y < y_range; _y++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", _y, y_range);
        for (int x = 0; x < image.Width(); x++)
        {
            int y = _y + offset;
            bool debug = false;
            if (y == 299 - 151 && x == 163 && parser.debug)
            {
                debug = true;
            }
            unsigned short seed[3] = {
                y,
                static_cast<unsigned short>(y * x),
                static_cast<unsigned short>(y * x * y)};
            Vector3f color;

            for (int i = 0; i < parser.spp; i++)
            {
                if (parser.jitter)
                {
                    auto dx = erand48(seed);
                    auto dy = erand48(seed);
                    Ray camRay = camera->generateRay(Vector2f(x - 0.5 + dx, y - 0.5 + dy), seed);
                    color += tracer->traceRay(camRay, t_min, 0, seed, ALL, debug);
                }
                else
                {
                    Ray camRay = camera->generateRay(Vector2f(x, y), seed);
                    color += tracer->traceRay(camRay, t_min, 0, seed, ALL, debug);
                }
            }
            image.SetPixel(x, _y, VectorUtils::clamp(color / parser.spp));
        }
    }
    fprintf(stderr, "\rprocessing %5d/%-5d\n", y_range, y_range);
    delete tracer;
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
    int height, width;
    if (parser.set_size)
        height = parser.height, width = parser.width;
    else
        height = scene.getCamera()->getHeight(), width = scene.getCamera()->getHeight();
    if (parser.jitter)
        height *= 3, width *= 3;

    int offset = parser.offset;
    int y_range = (parser.size ? parser.size : height - offset);

    auto image = parser.segments.empty() ? render(parser, &scene, height, width, offset, y_range) : merge(parser);

    if (parser.filter)
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
