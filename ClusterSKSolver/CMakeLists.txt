# CMakeList.txt: proyecto de CMake para ClusterSKSolver, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

# Agregue un origen al ejecutable de este proyecto.
add_executable (ClusterSKSolver "ClusterSKSolver.cpp" "ClusterSKSolver.h")

# TODO: Agregue pruebas y destinos de instalación si es necesario.

# Dependencia de spdlog
target_link_libraries(ClusterSKSolver spdlog::spdlog)