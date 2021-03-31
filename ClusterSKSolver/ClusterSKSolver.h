// ClusterSKSolver.h: archivo de inclusión para archivos de inclusión estándar del sistema,
// o archivos de inclusión específicos de un proyecto.
#pragma once

#include <iostream>

// TODO: Haga referencia aquí a los encabezados adicionales que el programa requiere.
#include <vector>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <mpi.h>

#include "header/SKLoader.h"
#include "header/SKSolver.h"

int main(int argc, char** argv);