#pragma once

#include "game_field.h"

game_field fast_parallel(game_field& initial_field, int n_iter_, int n_threads);
