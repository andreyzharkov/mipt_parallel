#include <vector>
#include <iostream>

#include "game_field.h"



std::vector<std::pair<int, int> > split_field(game_field field, int n_threads){
	std::vector<int> ends;
	int sz  = field.field.size();
	float step = sz / (float)n_threads;
	for (float i = step; i < (float)sz; i += step){
		ends.push_back(floor(i));
	}
	if (ends.size() == n_threads) ends.pop_back();
	ends.push_back(sz);
	
	std::vector<std::pair<int, int> > bounds;
	bounds.push_back(std::pair<int, int>(0, ends[0]));
	for (int i = 1; i < n_threads; i++){
		bounds.push_back(std::pair<int, int>((bounds[i-1]).second, ends[i]));
	}
	return bounds;
}

//recalculate next by prev in area [from, to)
void next(game_field& prev, game_field& next, int from, int to){
	for (int i = from; i < to; i++){
		for (int j = 0; j < prev.field[0].size(); j++){
			int neighbours = 0;
			for (int ii = -1; ii <= 1; ++ii)
				for (int jj = -1; jj <= 1; ++jj)
					neighbours += prev.get(i + ii, j + jj);
			if (prev.get(i, j) == 0){
				if (neighbours == 3) next.field[i][j] = 1;
				else next.field[i][j] = 0;
			}
			else{
				neighbours -= 1;
				if (neighbours >= 2 && neighbours <= 3) next.field[i][j] = 1;
				else next.field[i][j] = 0;
			}
		}
	}
}
