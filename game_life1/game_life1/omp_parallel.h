//#pragma once

#include <omp.h>
#include <iostream>

#include "game_field.h"

game_field omp_life(game_field initial_field, int n_iter, int n_threads){
	std::vector<std::pair<int, int> > bounds = split_field(initial_field, n_threads);
	game_field even = initial_field;
	game_field odd = initial_field;
	#pragma omp parallel num_threads(n_threads)
	{
		for (int i = 1; i <= n_iter; ++i){
			if (i % 2 == 1){
				next(even, odd, bounds[omp_get_thread_num()].first, bounds[omp_get_thread_num()].second);
			}
			else{
				next(odd, even, bounds[omp_get_thread_num()].first, bounds[omp_get_thread_num()].second);
			}
			#pragma omp barrier
		}
	}
	return (n_iter % 2 == 0) ? even : odd;
}
