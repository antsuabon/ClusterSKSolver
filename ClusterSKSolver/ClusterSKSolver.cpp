// ClusterSKSolver.cpp: define el punto de entrada de la aplicación.
//

#include "ClusterSKSolver.h"

using namespace std;

vector<string> tokenize(string s, string del = " ")
{
	vector<string> res;

	int start = 0;
	int end = s.find(del);
	while (end != -1) {
		res.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	res.push_back(s.substr(start, end - start));

	return res;
}

int main(int argc, char** argv)
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
	

	if (argc != 2)
	{
		spdlog::error("Num. of arguments is incorrect");
		return 1;
	}
	cout << "\n";

	int initialState[16][16] = {
		{0,0,10,0,0,0,0,13,0,6,16,0,0,0,1,0},
		{6,1,0,9,0,0,0,11,0,0,0,0,10,0,0,4},
		{0,14,15,5,0,7,10,0,0,0,0,3,0,0,12,6},
		{8,2,0,0,0,0,14,15,7,0,0,0,13,9,11,0},
		{0,0,2,0,16,12,9,7,15,0,5,0,3,0,6,0},
		{0,0,0,10,0,0,5,2,8,0,0,0,0,0,7,0},
		{12,3,0,0,14,15,13,0,0,7,0,0,8,0,0,1},
		{0,0,0,7,0,6,1,10,0,0,3,2,9,12,0,5},
		{0,0,0,0,13,1,0,0,2,0,0,8,0,14,0,0},
		{0,0,16,0,0,2,0,0,0,9,0,6,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,11,12,4,0,10,5,0},
		{0,9,0,14,0,0,0,0,0,10,7,0,16,0,0,2},
		{10,6,0,0,5,0,0,0,0,8,0,0,7,0,15,9},
		{0,0,3,11,2,16,0,0,0,12,1,0,6,0,0,0},
		{13,0,0,0,4,10,0,0,0,0,2,0,0,5,3,8},
		{0,0,0,0,7,0,0,0,3,16,13,0,1,0,2,0}
	};

	
	int rows = 16, cols = 16;

	// Copiado del array

	int* sudokuArray = new int[rows * cols];
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			sudokuArray[i * cols + j] = initialState[i][j];
		}
	}

	
	if (rank == 0)
	{
		// Imprime el estado inicial
		printState(sudokuArray, rows, cols);
		cout << "\n";
	}

	// Ejecuta el algoritmo secuencial
	spdlog::stopwatch sw;
	int steps = 0;
	int isSolved = solveSudoku(stoi(argv[1]), &steps, sudokuArray, rows, cols, 4, 4);
	cout << "\n";
	
	if (rank == 0)
	{
		// Imprime el estado final
		printState(sudokuArray, rows, cols);
		cout << "\n";

		spdlog::info("Found solution? {}", isSolved);
		spdlog::info("Elapsed time: {:.3}", sw);
		spdlog::info("Needed steps: {}", steps);
	}

	MPI_Finalize();
	


	/*

	MPI_Init(&argc, &argv);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	string a = "0,3,1,0";
	vector<string> hola = tokenize(a, ",");

	for each (string var in hola)
	{
		cout << var << endl;
	}
	*/

	/*
	int intialState[4][4] = {
		{0,3,1,0},
		{0,1,0,2},
		{3,0,0,0},
		{0,0,2,0}
	};
	
	
	int initialState[9][9] = {
		{0,0,0,2,6,0,7,0,1},
		{6,8,0,0,7,0,0,9,0},
		{1,9,0,0,0,4,5,0,0},
		{8,2,0,1,0,0,0,4,0},
		{0,0,4,6,0,2,9,0,0},
		{0,5,0,0,0,3,0,2,8},
		{0,0,9,3,0,0,0,7,4},
		{0,4,0,0,5,0,0,3,6},
		{7,0,3,0,1,8,0,0,0}
	};
	*/

	
	
	/*
	int initialState[9][9] = {
		{6,8,0,9,0,5,0,0,0},
		{0,0,3,0,0,0,5,0,8},
		{4,0,2,1,0,8,7,0,3},
		{3,9,0,7,2,0,8,0,0},
		{0,0,0,0,0,0,0,1,0},
		{0,4,5,0,0,6,9,0,0},
		{0,6,0,8,0,4,0,0,2},
		{0,0,1,0,0,2,0,7,5},
		{7,0,0,0,1,3,0,0,0}
	};

	int initialState[9][9] = {
		{0,0,4,3,0,0,2,0,9},
		{0,0,5,0,0,9,0,0,1},
		{0,7,0,0,6,0,0,4,3},
		{0,0,6,0,0,2,0,8,7},
		{1,9,0,0,0,7,4,0,0},
		{0,5,0,0,8,3,0,0,0},
		{6,0,0,0,0,0,1,0,5},
		{0,0,3,5,0,8,6,9,0},
		{0,4,2,9,1,0,3,0,0}
	};
	
	
	int initialState[16][16] = {
		{0,0,10,0,0,0,0,13,0,6,16,0,0,0,1,0},
		{6,1,0,9,0,0,0,11,0,0,0,0,10,0,0,4},
		{0,14,15,5,0,7,10,0,0,0,0,3,0,0,12,6},
		{8,2,0,0,0,0,14,15,7,0,0,0,13,9,11,0},
		{0,0,2,0,16,12,9,7,15,0,5,0,3,0,6,0},
		{0,0,0,10,0,0,5,2,8,0,0,0,0,0,7,0},
		{12,3,0,0,14,15,13,0,0,7,0,0,8,0,0,1},
		{0,0,0,7,0,6,1,10,0,0,3,2,9,12,0,5},
		{0,0,0,0,13,1,0,0,2,0,0,8,0,14,0,0},
		{0,0,16,0,0,2,0,0,0,9,0,6,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,11,12,4,0,10,5,0},
		{0,9,0,14,0,0,0,0,0,10,7,0,16,0,0,2},
		{10,6,0,0,5,0,0,0,0,8,0,0,7,0,15,9},
		{0,0,3,11,2,16,0,0,0,12,1,0,6,0,0,0},
		{13,0,0,0,4,10,0,0,0,0,2,0,0,5,3,8},
		{0,0,0,0,7,0,0,0,3,16,13,0,1,0,2,0}
	};
	*/


	

	

	
	
	/*
	if (rank == 0)
	{

		MPI_Send(sudokuArray, 81, MPI_INT, 1, 0, MPI_COMM_WORLD);

		MPI_Status status;
		MPI_Recv(sudokuArray, 81, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

		spdlog::info("=> Rank: {}", rank);
		printState(sudokuArray, rows, cols);
	}
	else
	{
		MPI_Status status;
		MPI_Recv(sudokuArray, 81, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		solveSudoku(sudokuArray, rows, cols, 3, 3);

		MPI_Send(sudokuArray, 81, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	*/
	

	//free(sudokuArray);
	//
	//

	/*
	
	

	MPI_Finalize();
	*/

	return 0;
}
