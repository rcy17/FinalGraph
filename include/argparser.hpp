#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tracer.hpp"
#include <list>

class ArgParser
{

public:
      ArgParser() { DefaultValues(); }

      ArgParser(int argc, char *argv[])
      {
            DefaultValues();

            for (int i = 1; i < argc; i++)
            {

                  // rendering output
                  if (!strcmp(argv[i], "-input"))
                  {
                        i++;
                        assert(i < argc);
                        input_file = argv[i];
                  }
                  else if (!strcmp(argv[i], "-output"))
                  {
                        i++;
                        assert(i < argc);
                        output_file = argv[i];
                  }
                  else if (!strcmp(argv[i], "-normals"))
                  {
                        i++;
                        assert(i < argc);
                        normals_file = argv[i];
                  }
                  else if (!strcmp(argv[i], "-size"))
                  {
                        set_size = 1;
                        i++;
                        assert(i < argc);
                        width = atoi(argv[i]);
                        i++;
                        assert(i < argc);
                        height = atoi(argv[i]);
                  }

                  // rendering options
                  else if (!strcmp(argv[i], "-depth"))
                  {
                        i++;
                        assert(i < argc);
                        depth_min = (double)atof(argv[i]);
                        i++;
                        assert(i < argc);
                        depth_max = (double)atof(argv[i]);
                        i++;
                        assert(i < argc);
                        depth_file = argv[i];
                  }
                  else if (!strcmp(argv[i], "-bounces"))
                  {
                        i++;
                        assert(i < argc);
                        bounces = atoi(argv[i]);
                  }
                  else if (!strcmp(argv[i], "-shadows"))
                  {
                        shadows = 1;
                  }
                  else if (!strcmp(argv[i], "-refractions"))
                  {
                        refractions = 1;
                  }

                  // path tracer
                  else if (strcmp(argv[i], "-pt") == 0)
                  {
                        type = PT;
                  }
                  else if (!strcmp(argv[i], "-spp"))
                  {
                        i++;
                        assert(i < argc);
                        spp = atoi(argv[i]);
                  }

                  // gamma correction
                  else if (!strcmp(argv[i], "-gamma"))
                  {
                        gamma = 1;
                  }

                  // y-range
                  else if (!strcmp(argv[i], "-range"))
                  {
                        i++;
                        assert(i < argc);
                        offset = atoi(argv[i]);
                        i++;
                        assert(i < argc);
                        size = atoi(argv[i]);
                  }

                  // merge files
                  else if (!strcmp(argv[i], "-merge"))
                  {
                        for (i++; i < argc && argv[i][0] != '-'; i++)
                        {
                              segments.push_back(argv[i]);
                        }
                        i--;
                  }

                  else if (!strcmp(argv[i], "-debug") || !strcmp(argv[i], "-d"))
                  {
                        debug = true;
                  }

                  else
                  {
                        printf("Unknown command line argument %d: '%s'\n", i, argv[i]);
                        assert(0);
                  }
            }
            if (type == RT)
                  spp = 1;

            if ((offset || size))
            {
                  assert(strcmp(".bmp", output_file + strlen(output_file) - 4) &&
                         "Distributed method only support tga, but you request bmp\n");
            }
            assert((segments.empty() || set_size) && "Merge file must provide size\n");
            assert(segments.size() || input_file && "No input provided!\n");
      }

      void DefaultValues()
      {
            // rendering output
            input_file = NULL;
            output_file = NULL;
            depth_file = NULL;
            normals_file = NULL;
            set_size = 0;
            width = 0;
            height = 0;
            stats = 0;

            // rendering options
            depth_min = 0;
            depth_max = 1;
            bounces = 0;
            shadows = 0;
            refractions = 0;
            type = RT;

            // sampling
            jitter = 0;
            filter = 0;

            // pt
            spp = 100;

            // gamma correct
            gamma = 0;

            // range for distributed calculation
            offset = 0;
            size = 0;

            segments.clear();
            debug = false;
      }

      // ==============
      // REPRESENTATION
      // all public! (no accessors)

      // rendering output
      char *input_file;
      char *output_file;
      char *depth_file;
      char *normals_file;
      int set_size;
      int width;
      int height;
      int stats;

      // rendering options
      double depth_min;
      double depth_max;
      int bounces;
      bool shadows;
      bool refractions;
      TraceType type;

      // supersampling
      bool jitter;
      bool filter;

      // Samples Per Pixel
      int spp;

      // Gamma Correction
      bool gamma;

      // Range
      int offset;
      int size;

      // load distributed data
      list<const char *> segments;

      // debug choice
      bool debug;
};

#endif // ARG_PARSER_H
