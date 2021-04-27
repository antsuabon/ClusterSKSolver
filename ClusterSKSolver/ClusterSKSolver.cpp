// ClusterSKSolver.cpp: define el punto de entrada de la aplicación.
//

#include "ClusterSKSolver.h"

using namespace std;

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		spdlog::info("Num. of arguments: {}", argc);
		spdlog::info("Program name: {}", argv[0]);

		for (size_t i = 1; i < argc; i++)
		{
			spdlog::info("Param. [{}]: {}", i, argv[i]);
		}
	}

	if (argc != 3)
	{
		spdlog::error("Num. of arguments is incorrect");
		return 1;
	}
	//cout << "\n";

	int rows, cols, regionX, regionY;
	map<vector<pair<int, int>>, int> blocks;
	int *sudokuArray;

	loadSudoku(argv[2], &sudokuArray, &blocks, &rows, &cols, &regionX, &regionY);

	//auto logger = spdlog::basic_logger_mt("basic_logger", "logs/example.log");
	//logger->set_pattern("%v");
	//logger->set_level(spdlog::level::off);
	/*
	if (rank == 0)
	{
		// Imprime el estado inicial
		printState(sudokuArray, rows, cols, regionX, regionY);
		cout << "\n";
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// Ejecuta el algoritmo secuencial
	spdlog::stopwatch sw;
	int steps = 0;
	int isSolved = DistributedSKSolver::solveSudoku(rank, size, stoi(argv[1]), &steps, sudokuArray, rows, cols, regionX, regionY);

	// SKSolver::solveSudoku(stoi(argv[1]), &steps, sudokuArray, rows,cols, regionX, regionY);

	// DistributedSKSolver::solveSudoku(rank, size, stoi(argv[1]), &steps, sudokuArray, rows, cols, regionX, regionY);

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		cout << "\n";
		spdlog::info("Found solution? {}", isSolved);
		spdlog::info("Elapsed time: {:.3}", sw);
		spdlog::info("Needed steps: {}", steps);
		cout << "\n";

		// Imprime el estado final
		printState(sudokuArray, rows, cols, regionX, regionY);
		//cout << "\n";
	}

	*/
	MPI_Finalize();

	return 0;
}
