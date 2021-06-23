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

	if (argc < 6)
	{
		spdlog::error("Num. of arguments is incorrect");
		return 1;
	}

	int algorithm = stoi(argv[2]);
	int heuristic = stoi(argv[3]);
	int auxParam = stod(argv[4]);

	if (algorithm < 0 || algorithm > 2)
	{
		if (rank == 0)
			spdlog::error("Not existing algorithm");

		return 1;
	}

	if (heuristic < 0 || heuristic > 3)
	{
		if (rank == 0)
			spdlog::error("Not existing heuristic");
		return 1;
	}

	if (algorithm > 0 && auxParam <= 0)
	{
		if (rank == 0)
			spdlog::error("Algorithm parameter is invalid");
		return 1;
	}

	if (algorithm > 0 && size == 1)
	{
		if (rank == 0)
			spdlog::error("Invalid number of slave nodes");
		return 1;
	}

	shared_ptr<spdlog::logger> logger = nullptr;

	if (stoi(argv[5]) > 0)
	{
		// Ficheros de log para la resolución paso a paso
		logger = spdlog::basic_logger_mt("basic_logger", "logs/log-" + to_string(rank) + ".log");
		logger->set_pattern("%v");
	}

	// Representación del problema
	int n, regionX, regionY;
	map<vector<pair<int, int>>, int> blocks;
	int *sudokuArray;

	loadSudoku(argv[1], &sudokuArray, &blocks, &n, &regionX, &regionY);

	if (rank == 0)
	{
		// Imprime el estado inicial
		printState(sudokuArray, n, regionX, regionY);
		cout << "\n";
	}
	MPI_Barrier(MPI_COMM_WORLD);

	spdlog::stopwatch sw;
	int steps = 0;
	int depth = 0;
	int isSolved = 1;

	// Selección del algoritmo utilizando el parámetro de entrada
	switch (algorithm)
	{
	case 0:
		// Ejecución secuencial del algoritmo
		isSolved = SKSolver::solveSudoku(heuristic, &steps, &depth, sudokuArray, n, regionX, regionY, blocks, sw, logger);
		break;

	case 1:
		// Ejecución secuencial del algoritmo con distribución estática de trabajos
		isSolved = CentralSKSolver::solveSudoku(rank, size, heuristic, auxParam, &steps, sudokuArray, n, regionX, regionY, blocks, sw, logger);
		break;

	case 2:
		// Ejecución secuencial del algoritmo con distribución dinámica de trabajos
		isSolved = DistributedSKSolver::solveSudoku(rank, size, heuristic, auxParam, &steps, sudokuArray, n, regionX, regionY, blocks, sw, logger);
		break;

	default:
		return 1;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		cout << "\n";
		spdlog::info("Found solution? {}", isSolved);
		spdlog::info("Elapsed time: {:.3}", sw);
		spdlog::info("Needed steps: {}", steps);
		cout << "\n";

		// Imprime el estado final
		printState(sudokuArray, n, regionX, regionY);
		//cout << "\n";
	}

	MPI_Finalize();

	return 0;
}
