#include "ComputerAI.hpp"
#include "GameLogic.hpp"
#include <vector>
#include <cstdlib>

using namespace std;
using namespace sf;

Move getEasyMove(Piesa players[], int turn, int curentVisitability[][DIM_TABLA]) {
	Move mutare;
	mutare.isMove = (rand() % 2 != 0);

	if (players[turn].pereti == 0)
		mutare.isMove = true;

	if (mutare.isMove) {
		vector<Vector2i> vecini;

		for (int i = 0; i < DIM_TABLA; i++)
			for (int j = 0; j < DIM_TABLA; j++)
				if (curentVisitability[i][j] == 1)
					vecini.push_back({ i, j });

		int index = rand() % vecini.size();
		mutare.pos = vecini[index];
	}
	else {
		vector<Vector2i> valid;
		mutare.wallOrientation = (rand() % 2 == 0) ? Orientation::Horizontal : Orientation::Vertical;

		for (int i = 0; i < DIM_TABLA; i++)
			for (int j = 0; j < DIM_TABLA; j++) {
				if (walls[i][j].hasHorizontal == false) { 
					if (walls[i][j].hasVertical) continue;
					if (j > 0 && walls[i][j - 1].hasVertical) continue;
					if (walls[i][j + 1].hasVertical) continue;
					if (walls[i][j].hasHorizontal) continue;
				}
				if (walls[i][j].hasVertical == false) { 
					if (walls[i][j].hasHorizontal) continue;
					if (i > 0 && walls[i - 1][j].hasHorizontal) continue;
					if (walls[i + 1][j].hasHorizontal) continue;
					if (walls[i][j].hasVertical) continue;
				}
				valid.push_back({ i,j });
			}

		if (valid.empty()) {
			mutare.isMove = true;
			vector<Vector2i> vecini;

			for (int i = 0; i < DIM_TABLA; i++)
				for (int j = 0; j < DIM_TABLA; j++)
					if (curentVisitability[i][j] == 1)
						vecini.push_back({ i, j });

			int index = rand() % vecini.size();
			mutare.pos = vecini[index];
		}
		else {
			int index = rand() % valid.size();
			mutare.pos = valid[index];
		}
	}
	return mutare;
}

Move getMediumMove(Piesa players[], int turn, int curentVisitability[][DIM_TABLA], int playerCount) {
	if (rand() % 2 == 0)
		return getHardMove(players, turn, curentVisitability, playerCount);
	else
		return getEasyMove(players, turn, curentVisitability);
}

Move getHardMove(Piesa players[], int turn, int curentVisitability[][DIM_TABLA], int playerCount) {
	Move bestMove;
	int myDist = 0;
	int minOpDist = 999;
	vector<int> opps;

	for (int i = 0; i < playerCount; i++) {
		Vector2i pos = { players[i].position.x, players[i].position.y };
		int distance = getDistance(pos, players[i].winCondition, i, playerCount);

		if (i == turn)
			myDist = distance;
		else {
			opps.push_back(i);
			if (distance < minOpDist)
				minOpDist = distance;
		}
	}

	bool shouldConsiderWall = (players[turn].pereti > 0) && (minOpDist <= myDist || minOpDist < 4);

	if (shouldConsiderWall) {
		int bestScore = -1000;
		Move bestWall;
		bool foundWall = false;

		for (int i = 0; i < DIM_TABLA - 1; i++) {
			for (int j = 0; j < DIM_TABLA - 1; j++) {
				for (int orient = 0; orient < 2; orient++) {
					if (orient == 1) { // vertical
						if (walls[i][j].hasVertical || walls[i][j].hasHorizontal) continue;
						if (j > 0 && walls[i][j - 1].hasVertical) continue;
						if (walls[i][j + 1].hasVertical) continue;
					}
					else { // orizontal
						if (walls[i][j].hasHorizontal || walls[i][j].hasVertical) continue;
						if (i > 0 && walls[i - 1][j].hasHorizontal) continue;
						if (walls[i + 1][j].hasHorizontal) continue;
					}

					if (orient == 1) walls[i][j].hasVertical = true;
					else walls[i][j].hasHorizontal = true;

					bool isMoveSafe = true;
					int newMyDist = getDistance({ players[turn].position.x, players[turn].position.y }, players[turn].winCondition, turn, playerCount);

					if (newMyDist > 900) isMoveSafe = false;

					int newMinOpDist = 999;
					for (int opponent : opps) {
						int distance = getDistance({ players[opponent].position.x, players[opponent].position.y }, players[opponent].winCondition, opponent, playerCount);
						if (distance > 900) {
							isMoveSafe = false;
							break;
						}
						if (distance < newMinOpDist) newMinOpDist = distance;
					}

					if (orient == 1) walls[i][j].hasVertical = false;
					else walls[i][j].hasHorizontal = false;

					if (isMoveSafe) {
						int score = (newMinOpDist - minOpDist) - (newMyDist - myDist);
						if (score > bestScore) {
							bestScore = score;
							bestWall.isMove = false;
                            bestWall.pos = {i, j};
                            bestWall.wallOrientation = (orient == 1) ? Orientation::Vertical : Orientation::Horizontal;
							if (score > 0) foundWall = true;
						}
					}
				}
			}
		}
		if (foundWall) return bestWall;
	}

	bestMove.isMove = true;
	int minDist = 999;
	vector<Vector2i> validMoves;

	for (int i = 0; i < DIM_TABLA; i++)
		for (int j = 0; j < DIM_TABLA; j++)
			if (curentVisitability[i][j] == 1)
				validMoves.push_back({ i, j });

	if(!validMoves.empty()) bestMove.pos = validMoves[0];
	int startIdx = validMoves.empty() ? 0 : rand() % validMoves.size();

	for (size_t k = 0; k < validMoves.size(); k++) {
		Vector2i p = validMoves[(startIdx + k) % validMoves.size()];
		int d = getDistance(p, players[turn].winCondition, turn, playerCount);

		if (d < minDist) {
			minDist = d;
			bestMove.pos = p;
		}
	}
	return bestMove;
}

int getDistance(Vector2i start, int target, int player, int playerCount) {
	int dist[DIM_TABLA][DIM_TABLA];
	for (int i = 0; i < DIM_TABLA; i++)
		for (int j = 0; j < DIM_TABLA; j++)
			dist[i][j] = -1;

	queue<Vector2i> q;
	q.push(start);
	dist[start.x][start.y] = 0;

	while (!q.empty()) {
		Vector2i curr = q.front();
		q.pop();

		if (playerCount == 2) {
			if (curr.y == target) return dist[curr.x][curr.y];
		}
		else {
			if (player == 0 || player == 2) {
				if (curr.y == target) return dist[curr.x][curr.y];
			}
			else {
				if (curr.x == target) return dist[curr.x][curr.y];
			}
		}

		for (int i = 0; i < 4; i++) {
			int nx = curr.x + dx[i];
			int ny = curr.y + dy[i];

			if (nx >= 0 && nx < DIM_TABLA && ny >= 0 && ny < DIM_TABLA && dist[nx][ny] == -1) {
				if (checkWallCollision(curr.x, curr.y, i)) {
					dist[nx][ny] = dist[curr.x][curr.y] + 1;
					q.push({ nx, ny });
				}
			}
		}
	}
	return 999; 
}