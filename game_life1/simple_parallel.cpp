#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>

#include "game_field.h"
#include "barrier.h"
#include "simple_parallel.h"

std::vector<std::pair<int, int> > bounds;
game_field current_field;
std::vector<std::vector<unsigned char> > next_field;
int curr_iter;
static int n_iter;
Barrier* barrier;

void change_current(){
	curr_iter++;
	std::swap(next_field, current_field.field);
}

void* thread_func(void *b){
	int left_border = ((std::pair<int, int> *)b)->first;
	int right_border = ((std::pair<int, int> *)b)->second;
	//std::cout << "ok" << std::endl;
	while (curr_iter < n_iter){
		for (int i = left_border; i < right_border; i++){
			for (int j = 0; j < next_field[0].size(); j++){
				int neighbours = 0;
				for (int ii = -1; ii <= 1; ++ii)
					for (int jj = -1; jj <= 1; ++jj)
						neighbours += current_field.get(i + ii, j + jj);
				if (current_field.get(i, j) == 0){
					if (neighbours == 3) next_field[i][j] = 1;
					else next_field[i][j] = 0;
				}
				else{
					neighbours -= 1;
					if (neighbours >= 2 && neighbours <=3) next_field[i][j] = 1;
					else next_field[i][j] = 0;
				}
			}
		}
		//std::cout << "before " << curr_iter << std::endl;
		barrier->enter();
		//std::cout << "after " << curr_iter << std::endl;
	}
}

game_field simple_parallel_life(game_field initial_field, int n_iterations, int n_threads){
	n_iter = n_iterations;
	curr_iter = 0;
	bounds = split_field(initial_field, n_threads);
	
	barrier = new Barrier(n_threads, change_current);
	next_field = initial_field.field;
	current_field = initial_field.field;
	
	std::vector<pthread_t> threads(n_threads - 1);
	for (int i = 0; i < n_threads - 1; i++){
		if(pthread_create(&threads[i], NULL, thread_func, &(bounds[i]))) {
            std::cout << "ERROR: CAN'T CREATE THREAD" << std::endl;
            exit(1);
		}
		std::cout << threads[i] << std::endl;
	}
	thread_func(&bounds[n_threads - 1]);
	int dum;
	std::cout << "before join" << std::endl;
	for (int i = 0; i < n_threads - 1; i++){
		std::cout << i << std::endl;
		std::cout << threads[i] << std::endl;
		if(pthread_join(threads[i],(void**)&dum)) {
            std::cout << "ERROR: CAN'T JOIN THREAD" << std::endl;
            exit(1);
		}
		std::cout << i << std::endl;
	}
	std::cout << "after join" << std::endl;
	delete barrier;
	return current_field;
}
