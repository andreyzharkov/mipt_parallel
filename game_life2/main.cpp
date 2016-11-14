#include <iostream>
#include <algorithm>
#include <vector>
#include <mpi.h>
#include <cassert>
#include <stdlib.h>

#include "game_field.h"

int n, m, n_iter, n_threads;
game_field initial_field;
game_field res_field;
MPI_Status status;

game_field master_func(){
	std::vector<std::pair<int, int> > bounds = split_field(initial_field, n_threads);
	
	std::vector<std::vector<unsigned char> > subfield(bounds[0].second + 2, std::vector<unsigned char>(m, 0));
	for (int i = 0; i < bounds[0].second + 1; ++i){
		subfield[i + 1] = initial_field.field[i];
	}
	game_field curr(subfield);
	game_field prev(subfield);
	
	//send to other processes start subfields
	for (int thread_id = 1; thread_id < n_threads; ++thread_id){
		int width = bounds[thread_id].second - bounds[thread_id].first + 2;
		MPI_Send(&width, 1, MPI_INT, thread_id, 0, MPI_COMM_WORLD);
		for (int i = bounds[thread_id].first - 1; i < bounds[thread_id].second + 1; ++i){
			if (i < n){
				MPI_Send(&initial_field.field[i][0], m, MPI_UNSIGNED_CHAR, thread_id, 1, MPI_COMM_WORLD);
			}
			else{
				//send vector of zeros for the last thread's last column
				MPI_Send(&curr.field[0][0], m, MPI_UNSIGNED_CHAR, thread_id, 1, MPI_COMM_WORLD);
			}
		}
	}
	
	for (int i = 1; i <= n_iter; ++i){
		if (i%2){
			next(prev, curr, 0, prev.field.size());
			MPI_Send(&curr.field[curr.field.size() - 2][0], m, MPI_UNSIGNED_CHAR, 1, 100, MPI_COMM_WORLD);
			MPI_Recv(&curr.field[curr.field.size() - 1][0], m, MPI_UNSIGNED_CHAR, 1, 100, MPI_COMM_WORLD, &status);
			curr.field[0] = std::vector<unsigned char>(m, 0);
		}
		else{
			next(curr, prev, 0, prev.field.size());
			MPI_Send(&prev.field[curr.field.size() - 2][0], m, MPI_UNSIGNED_CHAR, 1, 100, MPI_COMM_WORLD);
			MPI_Recv(&prev.field[curr.field.size() - 1][0], m, MPI_UNSIGNED_CHAR, 1, 100, MPI_COMM_WORLD, &status);
			prev.field[0] = std::vector<unsigned char>(m, 0);
		}
	}
	
	//getting result	
	std::vector<std::vector<unsigned char> > result;
	for (int i = 0; i < curr.field.size() - 2; ++i){
		if (n_iter % 2) {
			result.push_back(curr.field[i + 1]);
		}
		else {
			result.push_back(prev.field[i + 1]);
		}
	}
	int thread_id = 1;
	for (int i = bounds[0].second; i < n; ++i){
		if (bounds[thread_id].second == i) thread_id++;
		result.push_back(std::vector<unsigned char>(m));
		MPI_Recv(&result[result.size() - 1][0], m, MPI_UNSIGNED_CHAR, thread_id, 666, MPI_COMM_WORLD, &status); 
	}
	return game_field(result);
}

void slave_func(int rank){
	int width;
	MPI_Recv(&width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	std::vector<std::vector<unsigned char> > subfield(width, std::vector<unsigned char>(m));
	for (int i = 0; i < width; ++i){
		MPI_Recv(&subfield[i][0], m, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
	}
	game_field prev(subfield);
	game_field curr(subfield);
	
	if (rank != n_threads - 1){
		for (int i = 1; i <= n_iter; ++i){
			if (i%2){
				next(prev, curr, 0, prev.field.size());
				MPI_Send(&curr.field[curr.field.size() - 2][0], m, MPI_UNSIGNED_CHAR, rank + 1, 100, MPI_COMM_WORLD);
				MPI_Send(&curr.field[1][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD);
				MPI_Recv(&curr.field[curr.field.size() - 1][0], m, MPI_UNSIGNED_CHAR, rank + 1, 100, MPI_COMM_WORLD, &status);
				MPI_Recv(&curr.field[0][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD, &status);
			}
			else{
				next(curr, prev, 0, prev.field.size());
				MPI_Send(&prev.field[curr.field.size() - 2][0], m, MPI_UNSIGNED_CHAR, rank + 1, 100, MPI_COMM_WORLD);
				MPI_Send(&prev.field[1][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD);
				MPI_Recv(&prev.field[curr.field.size() - 1][0], m, MPI_UNSIGNED_CHAR, rank + 1, 100, MPI_COMM_WORLD, &status);
				MPI_Recv(&prev.field[0][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD, &status);
			}
		}
	}
	else{
		for (int i = 1; i <= n_iter; ++i){
			if (i%2){
				next(prev, curr, 0, prev.field.size());
				MPI_Send(&curr.field[1][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD);
				MPI_Recv(&curr.field[0][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD, &status);
				curr.field[curr.field.size() - 1] = std::vector<unsigned char>(m, 0);
			}
			else{
				next(curr, prev, 0, prev.field.size());
				MPI_Send(&prev.field[1][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD);
				MPI_Recv(&prev.field[0][0], m, MPI_UNSIGNED_CHAR, rank - 1, 100, MPI_COMM_WORLD, &status);
				prev.field[curr.field.size() - 1] = std::vector<unsigned char>(m, 0);
			}
		}
	}
	
	for (int i = 1; i < width - 1; ++i){
		if (n_iter % 2){
			MPI_Send(&curr.field[i][0], m, MPI_UNSIGNED_CHAR, 0, 666, MPI_COMM_WORLD);
		}
		else{
			MPI_Send(&prev.field[i][0], m, MPI_UNSIGNED_CHAR, 0, 666, MPI_COMM_WORLD);
		}
	}
}

int main(int argc, char** argv){
	assert(argc > 3);
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	if (n < m) std::swap(n, m);
	n_iter = atoi(argv[3]);

	MPI_Init(&argc, &argv);
	
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_threads);
	assert(n_threads <= n);
	if (rank == 0){
		srand(time(NULL));
		initial_field = generate_start_field(n, m);
		//initial_field.print();
		double t = MPI_Wtime();
		res_field = simple_life(initial_field, n_iter);
		//res_field.print();
		t = MPI_Wtime() - t;
		std::cout << "non-parallel time: " << t << std::endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0){
		double t = MPI_Wtime();
		game_field res = master_func();
		//res.print();
		t = MPI_Wtime() - t;
		std::cout << "mpi time: " << t << std::endl;
		
		assert(res_field.field == res.field);
	}
	else {
		slave_func(rank);
	}
	
	MPI_Finalize();
	return 0;
}
