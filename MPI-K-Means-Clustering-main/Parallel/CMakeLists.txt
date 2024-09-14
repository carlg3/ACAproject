cmake_minimum_required(VERSION 3.22.1)
project(Parallel_proj_test)

set(CMAKE_CXX_STANDARD 20)

# Add MPI Package to Project
find_package(MPI REQUIRED)

include_directories(.)

add_executable(Parallel
    Centroid.cpp
    Centroid.h
    Cluster.cpp
    Cluster.h
    main.cpp
    Point.cpp
    Point.h
    Tupla.cpp
    Tupla.h)

# Link MPI libraries
target_link_libraries(Parallel PRIVATE MPI::MPI_CXX)