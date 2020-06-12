CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PA5 CXX)

FIND_PACKAGE( OpenMP REQUIRED)
IF(OPENMP_FOUND)
        MESSAGE("OPENMP FOUND")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(PA5_SOURCES
        src/cube_map.cpp
        src/image.cpp
        src/main.cpp
        src/material.cpp
        src/mesh.cpp
        src/noise.cpp
        src/octree.cpp
        src/perlin_noise.cpp
        src/ray_tracer.cpp
        src/scene_parser.cpp
        src/texture.cpp        
        )

SET(PA5_INCLUDES
        include/argparser.hpp
        include/bitmap_image.hpp
        include/camera.hpp
        include/cube_map.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/noise.hpp
        include/object3d.hpp
        include/octree.hpp
        include/perlin_noise.hpp
        include/plane.hpp
        include/ray.hpp
        include/ray_tracer.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/texture.hpp
        include/vector_utils.hpp
        )

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${PA5_SOURCES} ${PA5_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
ADD_DEFINITIONS("-Wall -g")
