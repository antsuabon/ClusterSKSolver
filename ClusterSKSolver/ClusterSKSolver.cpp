// ClusterSKSolver.cpp: define el punto de entrada de la aplicación.
//

#include "ClusterSKSolver.h"

using namespace std;

int main(int* argc, char*** argv)
{
	MPI_Init(argc, argv);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	spdlog::info("Hello {}", rank);

	MPI_Finalize();

	return 0;
}
