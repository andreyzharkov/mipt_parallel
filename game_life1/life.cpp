#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cassert>
#include <omp.h>

#include "game_field.h"
#include "simple_parallel.h"
#include "fast_parallel.h"
#include "omp_parallel.h"

int n, m;
bool is_test = true;

game_field next(game_field prev){
	std::vector<std::vector<unsigned char> > field(n, 
		std::vector<unsigned char>(m));
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++){
			int neighbours = 0;
			for (int ii = -1; ii <= 1; ++ii)
				for (int jj = -1; jj <= 1; ++jj)
					neighbours += prev.get(i + ii, j + jj);
			if (prev.get(i, j) == 0){
				if (neighbours == 3) field[i][j] = 1;
				else field[i][j] = 0;
			}
			else{
				neighbours -= 1;
				if (neighbours >= 2 && neighbours <=3) field[i][j] = 1;
				else field[i][j] = 0;
			}
		}
	}
	return game_field(field);
}

game_field generate_start_field(){
	std::vector<std::vector<unsigned char> > field(n, 
		std::vector<unsigned char>(m, rand() % 2));
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			field[i][j] = rand() % 2;
	return game_field(field);
}

game_field simple_life(game_field& initial, int n_iter){
	game_field curr_field = initial;
	for (int i = 0; i < n_iter; ++i){
		curr_field = next(curr_field);
	}
	return curr_field;
}

int main(int argc, char** argv) {
	srand((unsigned int) time(0));
	
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	int n_iter = atoi(argv[3]);
	int n_threads = atoi(argv[4]);
	
	assert(n > 0);
	assert(m > 0);
	assert(n_iter > 0);
	assert(n_threads > 0);
	
	
	double t;
	
	
	if (n > m) std::swap(n, m);
	
	game_field initial_field = generate_start_field();
	
	//////////////////
	//////////////////
	
    struct timespec begin, end;
    long elapsed_seconds;
    
    t = omp_get_wtime();
    game_field non_parallel_res = simple_life(initial_field, n_iter);
    t = omp_get_wtime() - t;
    
    std::cout << t << ",";
    
    t = omp_get_wtime();
	//game_field simple_parallel_res = simple_parallel_life(initial_field, n_iter, n_threads);
    t = omp_get_wtime() - t;
    
    //assert(non_parallel_res.field == simple_parallel_res.field);
    
    //std::cout << t << "," << std::endl;

	t = omp_get_wtime();
	game_field omp_res = omp_life(initial_field, n_iter, n_threads);
    t = omp_get_wtime() - t;
    
    assert(non_parallel_res.field == omp_res.field);
    
    std::cout << t << ",";
    
    t = omp_get_wtime();
	game_field fast_parallel_res = fast_parallel(initial_field, n_iter, n_threads);
    t = omp_get_wtime() - t;
    
    assert(non_parallel_res.field == fast_parallel_res.field);
    
    std::cout << t << std::endl;
	
	//////////////////
	//////////////////
	
 /*   struct timespec begin, end;
    long elapsed_seconds;
    
    clock_gettime(CLOCK_REALTIME, &begin);
    game_field ans_s_simple = simple_life(initial_field, n_iter);
    clock_gettime(CLOCK_REALTIME, &end);
    
    elapsed_seconds = end.tv_sec - begin.tv_sec;
    std::cout << elapsed_seconds << ",";
    
    
    clock_gettime(CLOCK_REALTIME, &begin);
    game_field ans_p_simple = simple_parallel_life(initial_field, n_iter, n_threads);
    clock_gettime(CLOCK_REALTIME, &end);
    
    elapsed_seconds= end.tv_sec - begin.tv_sec;
    std::cout << elapsed_seconds << ",";
    
    
    clock_gettime(CLOCK_REALTIME, &begin);
    game_field ans_p_fine = fast_parallel(initial_field, n_iter, n_threads);
    clock_gettime(CLOCK_REALTIME, &end);
    
    elapsed_seconds = end.tv_sec - begin.tv_sec;
    std::cout << elapsed_seconds << std::endl;
	*/
	/*
	//std::chrono::time_point<std::chrono::system_clock> start, end;
	//int milliseconds_diff;
	
	//non-parallel
	//start = std::chrono::system_clock::now();
	//game_field non_parallel_res = simple_life(initial_field, n_iter);
	end = std::chrono::system_clock::now();
	
	//
	//
	//non_parallel_res.print();
	//
	
	milliseconds_diff = std::chrono::duration_cast<std::chrono::milliseconds>
	(end-start).count();
	
	if (!is_test){
		std::cout << "non-parallel time: " << milliseconds_diff << " ms" << std::endl;
	}
	else{
		std::cout << milliseconds_diff << ", ";
	}
	//simple parallel
	start = std::chrono::system_clock::now();
	game_field simple_parallel_res = simple_parallel_life(initial_field, n_iter, n_threads);
	end = std::chrono::system_clock::now();
	
	milliseconds_diff = std::chrono::duration_cast<std::chrono::milliseconds>
	(end-start).count();
	
	if (!is_test){
		std::cout << "simple-parallel time: " << milliseconds_diff << " ms" << std::endl;
	}
	else{
		std::cout << milliseconds_diff << ", ";
	}
	assert(non_parallel_res.field == simple_parallel_res.field);
	//good parallel
	start = std::chrono::system_clock::now();
	game_field fast_parallel_res = fast_parallel(initial_field, n_iter, n_threads);
	end = std::chrono::system_clock::now();
	
	milliseconds_diff = std::chrono::duration_cast<std::chrono::milliseconds>
	(end-start).count();
	
	if (!is_test){
		std::cout << "fast-parallel time: " << milliseconds_diff << " ms" << std::endl;
	}
	else{
		std::cout << milliseconds_diff << std::endl;
	}
	//
	//
	//fast_parallel_res.print();
	//
	assert(non_parallel_res.field == fast_parallel_res.field);
		
	if (!is_test) std::cout << "OK" << std::endl;
	*/
	return 0;
}
