# CMakeList.txt: proyecto de CMake para ClusterSKSolver, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required(VERSION 3.8)

# Headers directory
include_directories(header)

# Src files
set(CLUSTER_SK_SOLVER_SOURCES 
  "ClusterSKSolver.cpp"
  "source/SKSolver.cpp"
  "source/SKUtils.cpp"
  "source/SKLoader.cpp"
  "source/CentralSKSolver.cpp"
  "source/DistributedSKSolver.cpp")

# Agregue un origen al ejecutable de este proyecto.
add_executable (ClusterSKSolver  "ClusterSKSolver.h" ${CLUSTER_SK_SOLVER_SOURCES})

# TODO: Agregue pruebas y destinos de instalación si es necesario.

# Dependencia de spdlog
target_link_libraries(ClusterSKSolver spdlog::spdlog)

# Dependencia de MPI
target_link_libraries(ClusterSKSolver ${MPI_C_LIBRARIES})