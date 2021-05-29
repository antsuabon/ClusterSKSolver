# CMakeList.txt: archivo del proyecto de CMake de nivel superior, establezca la configuración global
# e incluya los subproyectos aquí.
#
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)

project(ClusterSKSolver)

# add_subdirectory()


find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
include_directories(external/spdlog/include)

# find_package(spdlog)

# Incluya los subproyectos.
add_subdirectory(ClusterSKSolver)