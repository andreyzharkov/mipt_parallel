#pragma once

#include <vector>

#include "game_field.h"

game_field simple_parallel_life(game_field initial_field, int n_iterations, int n_threads);
