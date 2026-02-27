#pragma once
#include "Constants.hpp"
#include "Entities.hpp"
#include "History.hpp"
#include <queue>

Move getEasyMove(Piesa[], int, int[][DIM_TABLA]);
Move getMediumMove(Piesa players[], int turn, int curentVisitability[][DIM_TABLA], int playerCount);
Move getHardMove(Piesa players[], int turn, int curentVisitability[][DIM_TABLA], int playerCount);
int getDistance(Vector2i start, int target, int player, int playerCount);