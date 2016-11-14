#pragma once

#include <vector>
#include <iostream>
#include <math.h>

struct game_field{
	std::vector<std::vector<unsigned char> > field;
	unsigned char get(int i, int j){
		if (i < 0 || j < 0 || i >= field.size() || j >= field[0].size()){
			return 0;
		}
		else return field[i][j];
	}
	void print(){
		std::cout << "size = (" << field.size() << ", " << field[0].size()
			<< ")" << std::endl;
		for (int i = 0; i < field.size(); i++){
			for (int j = 0; j < field[0].size(); j++){
				std::cout << (int)field[i][j];
			}
			std::cout << std::endl;
		}
	}
	game_field(std::vector<std::vector<unsigned char> >& f): field(f){}
	game_field(){}
};

std::vector<std::pair<int, int> > split_field(game_field field, int n_threads);

//recalculate next by prev in area [from, to)
void next(game_field& prev, game_field& next, int from, int to);
