#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [ ! -d build ]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
bin/PA5 -input testcases/scene01_basic.txt -output output/scene01.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene02_cube.txt -output output/scene02.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene03_sphere.txt -output output/scene03.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene04_axes.txt -output output/scene04.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene05_bunny_200.txt -output output/scene05.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene06_bunny_1k.txt -output output/scene06.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene07_shine.txt -output output/scene07.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene08_c.txt -output output/scene08.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene09_s.txt -output output/scene09.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene10_sphere.txt -output output/scene10.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene11_cube.txt -output output/scene11.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene12_vase.txt -output output/scene12.bmp -shadows -bounces 4 -refractions -jitter
bin/PA5 -input testcases/scene13_diamond.txt -output output/scene13.bmp -shadows -bounces 4 -refractions -jitter
