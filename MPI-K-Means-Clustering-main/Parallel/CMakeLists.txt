cmake_minimum_required(VERSION 3.22.1)
project(Parallel_proj_test)

set(CMAKE_CXX_STANDARD 20)

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(Parallel_proj_test
        Centroid.cpp
        Centroid.h
        Cluster.cpp
        Cluster.h
        Point.cpp
        Point.h
        Tupla.cpp
        Tupla.h
        kmeans_parallel.cpp)

# Link MPI libraries
target_link_libraries(Parallel_proj_test PRIVATE MPI::MPI_CXX)