cmake_minimum_required(VERSION 3.22.1)
project(Serial_proj_test)

set(CMAKE_CXX_STANDARD 20)

add_executable(Serial_proj_test
        Centroid.h
        Point.h
        Tupla.h
        Cluster.cpp
        Cluster.h
        Centroid.cpp
        Point.cpp
        Tupla.cpp
        kmeans_sequential.cpp
)