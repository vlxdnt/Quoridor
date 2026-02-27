#include "GameLogic.hpp"
#include "ComputerAI.hpp"
#include <cmath>
#include <queue>

using namespace sf;
using namespace std;

void runPlayerLogic(const Event& event, float startX, float startY, bool& isGameRunning, int& turn, int playerCount,
	Piesa* players, int curentVisitability[DIM_TABLA][DIM_TABLA], RenderTexture& wallsTexture, History& history, bool& showErrorMessage) {

	if (const auto* mouseEvent = event.getIf<Event::MouseButtonPressed>()) {
		if (mouseEvent->button == Mouse::Button::Left) {
			float relX = mouseEvent->position.x - startX - CELL_GAP;
			float relY = mouseEvent->position.y - startY - CELL_GAP;
			float totalCellSize = DIM_CELL + CELL_GAP;

			Vector2i click;
			click.x = (int)(relX / totalCellSize);
			click.y = (int)(relY / totalCellSize);

			float offsetX = fmod(relX, totalCellSize);
			float offsetY = fmod(relY, totalCellSize);
			bool isInsideCell = (offsetX < DIM_CELL && offsetY < DIM_CELL);

			if (click.x >= 0 && click.x < DIM_TABLA && click.y >= 0 && click.y < DIM_TABLA && isInsideCell) {
				Piesa* playerCurent = &players[turn];

				if (isMoveValid(curentVisitability, click)) {
					playerCurent->position.x = click.x;
					playerCurent->position.y = click.y;
					isGameRunning = !hasWon(*playerCurent, turn, playerCount);
                    showErrorMessage = false;

					if (isGameRunning) {
                        history.addMove(turn, true, click.x, click.y, false, colorPallete[players[turn].indexCuloare]);
						turn = (turn + 1) % playerCount;
						computeVisitable(curentVisitability, players, playerCount, turn); 
					}
				}
			}
		}

		if (mouseEvent->button == Mouse::Button::Right) {
			float relX = mouseEvent->position.x - startX - CELL_GAP;
			float relY = mouseEvent->position.y - startY - CELL_GAP;
			float totalCellSize = DIM_CELL + CELL_GAP;

			Vector2i click;
			click.x = (int)(relX / totalCellSize);
			click.y = (int)(relY / totalCellSize);

			float offsetX = fmod(relX, totalCellSize);
			float offsetY = fmod(relY, totalCellSize);

			if (click.x >= 0 && click.x < BOARD_SIZE && click.y >= 0 && click.y < BOARD_SIZE) {
				bool inGapX = (offsetX >= DIM_CELL);
				bool inGapY = (offsetY >= DIM_CELL);
				bool success = false;
				bool attempted = false;

				if (inGapX && !inGapY) {
					if (click.x >= 0 && click.x < (int)DIM_TABLA && click.y >= 0 && click.y < (int)DIM_TABLA) {
						success = placeWall(click.x, click.y, 1, wallsTexture, &players[turn], players, playerCount);
						attempted = true;
                    }
				}
				else if (!inGapX && inGapY) {
					if (click.x >= 0 && click.x < (int)DIM_TABLA && click.y >= 0 && click.y < (int)DIM_TABLA) {
						success = placeWall(click.x, click.y, 0, wallsTexture, &players[turn], players, playerCount);
						attempted = true;
                    }
				}
				if (success) {
                    showErrorMessage = false;
                    history.addMove(turn, false, click.x, click.y, walls[click.x][click.y].hasVertical, colorPallete[players[turn].indexCuloare]);
					turn = (turn + 1) % playerCount;
					computeVisitable(curentVisitability, players, playerCount, turn);
				}
				else if (attempted) {
					showErrorMessage = true;
				}
			}
		}
	}
}

void runComputerLogic(Piesa players[], int playerCount, int& turn, int curentVisitability[][DIM_TABLA],
	RenderTexture& wallsTexture, bool& isGameRunning, History& history, bool& timerStarted) {

	static Clock delayMove;
	Move mutare;

	if (!timerStarted) {
		delayMove.restart();
		timerStarted = true;
		return;
	}

	if (delayMove.getElapsedTime().asSeconds() < 1.0f)
		return;

	timerStarted = false;
	bool successful = true;

	if (players[turn].isBot == 1) {
		mutare = getEasyMove(players, turn, curentVisitability);
	}
	else if (players[turn].isBot == 2) {
		mutare = getMediumMove(players, turn, curentVisitability, playerCount);
	}
	else if (players[turn].isBot == 3){
		mutare = getHardMove(players, turn, curentVisitability, playerCount);
	}

	if (mutare.isMove) {
		players[turn].position.x = mutare.pos.x;
		players[turn].position.y = mutare.pos.y;
	}
	else {
        int orientInt = (mutare.wallOrientation == Orientation::Vertical) ? 1 : 0;
		successful = placeWall(mutare.pos.x, mutare.pos.y, orientInt, wallsTexture, &players[turn], players, playerCount);
    }

	if (!successful) {
        mutare.isMove = true;
		vector<Vector2i> vecini;

		for (int i = 0; i < DIM_TABLA; i++)
			for (int j = 0; j < DIM_TABLA; j++)
				if (curentVisitability[i][j] == 1)
					vecini.push_back({ i, j });

		int index = rand() % vecini.size();
		players[turn].position.x = vecini[index].x;
		players[turn].position.y = vecini[index].y;
		mutare.pos.x = vecini[index].x;
		mutare.pos.y = vecini[index].y;
		successful = true;
	}

    bool wOrientation = (mutare.wallOrientation == Orientation::Vertical);
    history.addMove(turn, mutare.isMove, mutare.pos.x, mutare.pos.y, wOrientation, colorPallete[players[turn].indexCuloare]);

    isGameRunning = !hasWon(players[turn], turn, playerCount);
    if (isGameRunning) {
        turn = (turn + 1) % playerCount;
        computeVisitable(curentVisitability, players, playerCount, turn);
    }
}

void resetGame(Piesa players[], int playerCount, int& turn, bool& isGameRunning, RenderTexture& wallsTexture, int visitability[][DIM_TABLA]) {
	turn = 0;
	isGameRunning = true;

	if (playerCount >= 2) {
		players[0] = { players[0].name, {4, 8}, 0, players[0].indexCuloare, numarPereti / playerCount, players[0].isBot};
		players[1] = { players[1].name, {4, 0}, 8, players[1].indexCuloare, numarPereti / playerCount, players[1].isBot};
	}
	if (playerCount == 4) {
		players[1] = { players[1].name, {0, 4}, 8, players[1].indexCuloare, numarPereti / playerCount, players[1].isBot};
		players[2] = { players[2].name, {4, 0}, 8, players[2].indexCuloare, numarPereti / playerCount, players[2].isBot};
		players[3] = { players[3].name, {8, 4}, 0, players[3].indexCuloare, numarPereti / playerCount, players[3].isBot};
	}

	for (int i = 0; i < DIM_TABLA; i++) {
		for (int j = 0; j < DIM_TABLA; j++) {
			walls[i][j].hasHorizontal = false;
			walls[i][j].hasVertical = false;
		}
	}
	wallsTexture.clear(Color::Transparent);
	computeVisitable(visitability, players, playerCount, turn);
}

bool isPathClear(Piesa player, int playerIndex, int playerCount) {
    bool visited[DIM_TABLA][DIM_TABLA] = { false };
    queue<Vector2i> coada;

    coada.push({ player.position.x, player.position.y });
    visited[player.position.x][player.position.y] = true;

    while (!coada.empty()) {
        Vector2i curent = coada.front();
        coada.pop();

    	if (playerCount == 2) {
    		if (curent.y == player.winCondition) return true;
    	}
    	else {
    		if (playerIndex == 0 || playerIndex == 2) {
    			if (curent.y == player.winCondition) return true;
    		}
    		else {
    			if (curent.x == player.winCondition) return true;
    		}
    	}

        for (int i = 0; i < 4; i++) {
            int nx = curent.x + dx[i];
            int ny = curent.y + dy[i];

            if (nx >= 0 && nx < DIM_TABLA && ny >= 0 && ny < DIM_TABLA) {
                if (!visited[nx][ny]) {
                    if (checkWallCollision(curent.x, curent.y, i)) {
                        visited[nx][ny] = true;
                        coada.push({ nx, ny });
                    }
                }
            }
        }
    }
    return false;
}

bool placeWall(int x, int y, int orientation, RenderTexture& rdTexture, Piesa* playerCurent, Piesa players[], int playerCount) {
	if (playerCurent->pereti <= 0) return false;
	if (x >= DIM_TABLA - 1 || y >= DIM_TABLA - 1) return false;

	if (orientation == 1) { // vertical
		if (walls[x][y].hasVertical) return false;
		if (y > 0 && walls[x][y - 1].hasVertical) return false;
		if (walls[x][y + 1].hasVertical) return false;
		if (walls[x][y].hasHorizontal) return false;
	}
	else { // orizontal
		if (walls[x][y].hasHorizontal) return false;
		if (x > 0 && walls[x - 1][y].hasHorizontal) return false;
		if (walls[x + 1][y].hasHorizontal) return false;
		if (walls[x][y].hasVertical) return false;
	}

	if (orientation == 1) walls[x][y].hasVertical = true;
	else walls[x][y].hasHorizontal = true;

	bool pathBlocked = false;
	for (int i = 0; i < playerCount; i++) {
		if (!isPathClear(players[i], i, playerCount)) {
			pathBlocked = true;
			break;
		}
	}

	if (pathBlocked) {
		if (orientation == 1) walls[x][y].hasVertical = false;
		else walls[x][y].hasHorizontal = false;
		return false;
	}

	float drawX, drawY;
	Vector2f Size;

	if (orientation == 1) { // vertical
		Size = Vector2f(-WALL_WIDTH, WALL_LENGTH);
		drawX = CELL_GAP + (x + 1) * (DIM_CELL + CELL_GAP);
		drawY = BOARD_SIZE - (y + 2) * (DIM_CELL + CELL_GAP);
	}
	else { // orizontal
		Size = Vector2f(WALL_LENGTH, -WALL_WIDTH);
		drawX = CELL_GAP + x * (DIM_CELL + CELL_GAP);
		drawY = BOARD_SIZE - (y + 1) * (DIM_CELL + CELL_GAP);
	}
	RectangleShape perete(Size);
	perete.setPosition({ drawX, drawY });
	perete.setFillColor(Color(153, 72, 29));

	rdTexture.draw(perete);
	playerCurent->pereti--;
	return true;
}

bool checkWallCollision(int x, int y, int direction) {
    switch (direction) {
    case 0: // SUS
        if (y > 0) {
            if (walls[x][y - 1].hasHorizontal) return false;
            if (x > 0 && walls[x - 1][y - 1].hasHorizontal) return false;
        }
        else return false;
        break;
    case 1: // DREAPTA
        if (walls[x][y].hasVertical) return false;
        if (y > 0 && walls[x][y - 1].hasVertical) return false;
        if(x >= DIM_TABLA - 1) return false;
        break;
    case 2: // JOS
        if (walls[x][y].hasHorizontal) return false;
        if (x > 0 && walls[x - 1][y].hasHorizontal) return false;
        if(y >= DIM_TABLA - 1) return false;
        break;
    case 3: // STANGA
        if (x > 0) {
            if (walls[x - 1][y].hasVertical) return false;
            if (y > 0 && walls[x - 1][y - 1].hasVertical) return false;
        }
        else return false;
        break;
    }
    return true; 
}

bool hasWon(Piesa player, int turn, int playerCount) {
	if (playerCount == 2) {
		return player.position.y == player.winCondition;
	}
	else {
		if (turn == 0 || turn == 2) return player.position.y == player.winCondition;
		else return player.position.x == player.winCondition;
	}
}

void computeVisitable(int visitability[][DIM_TABLA], Piesa players[], int playerCount, int turn) {
	for (int i = 0; i < DIM_TABLA; i++)
		for (int j = 0; j < DIM_TABLA; j++)
			visitability[i][j] = 0;

	for (int i = 0; i < playerCount; i++)
		visitability[players[i].position.x][players[i].position.y] = 2;
	visitability[players[turn].position.x][players[turn].position.y] = 3;

	int distanceX, distanceY, prim = 0, ultim = 0;
	Vector2i coada[4];
	int dir[3];

	coada[ultim].x = players[turn].position.x;
	coada[ultim++].y = players[turn].position.y;

	while (prim < ultim) {
		distanceX = coada[prim].x - players[turn].position.x;
		distanceY = coada[prim].y - players[turn].position.y;

		for (int i = 0; i < 4; i++) {
			if (distanceX == 0 || distanceY == 0)
				if (coada[prim].x + dx[i] < DIM_TABLA && coada[prim].y + dy[i] < DIM_TABLA
					&& coada[prim].x + dx[i] >= 0 && coada[prim].y + dy[i] >= 0) { 

					if (visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]] == 2
						&& checkWallCollision(coada[prim].x, coada[prim].y, i)) { 

						if (prim == 0) { 
							coada[ultim].x = coada[prim].x + dx[i];
							coada[ultim++].y = coada[prim].y + dy[i]; 
							dir[ultim - 2] = i; 
							visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]]++; 
						}
					}

					if (visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]] == 0
						&& checkWallCollision(coada[prim].x, coada[prim].y, i)) { 

						if (prim == 0) 
							visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]] = 1;
						else if (dir[prim - 1] == i) {
							if (checkWallCollision(coada[prim].x, coada[prim].y, i))
								visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]] = 1;
						}
						else if (!checkWallCollision(coada[prim].x, coada[prim].y, dir[prim - 1]))
							visitability[coada[prim].x + dx[i]][coada[prim].y + dy[i]] = 1;
					}
				}
		}
		prim++;
	}
}

bool isMoveValid(int visitability[][DIM_TABLA], Vector2i target) {
	return (visitability[target.x][target.y] == 1);
}

void initializePlayers(Piesa players[]) {
    players[0] = { "Player 1", {4, 8}, 0, 0, 10, 0};
    players[1] = { "Player 2", {0, 4}, 8, 1, 10, 0};
    players[2] = { "Player 3", {4, 0}, 8, 2, 10, 0};
    players[3] = { "Player 4", {8, 4}, 0, 3, 10, 0};
}

void initializeColorSelector(selectorButtons &colors, Font &font) {
    colors.verticalOffset = 2;
    Button clr(font, "", {25,25}, {0,0}, Color(40,60,80));
    for(Color cu : colorPallete) {
        clr.setFillColor(cu);
        colors.addOption(clr);
    }
}

void initializeAiSelector(selectorButtons &AI, Font &font) {
    AI.verticalOffset = 8;
    Button ai(font, "oprit", {80,30}, {0,0}, Color(40,60,80));
    ai.setFillColor(background);
    AI.addOption(ai);
    
    ai.setString("usor");
    ai.setSize({70,30});
    AI.addOption(ai);
    
    ai.setString("mediu");
    ai.setSize({120,30});
    AI.addOption(ai);
    
    ai.setString("greu");
    ai.setSize({70,30});
    AI.addOption(ai);
}

void audioLevelSprite(RectangleShape& aud, RectangleShape& volRect, Texture audioTextures[], Music& backgroundMusic) {
    float volume = backgroundMusic.getVolume();
    volRect.setSize({2*volume,10});
    int i = 0;
    if(volume >= 50) {
        if(volume >= 75) i = 4;
        else i = 3;
    }
    else {
        if(volume >= 25) i = 2;
        else {
            if(volume > 0) i = 1;
            else i = 0;
        }
    }

    aud.setSize({audioTextures[i].getSize().x/2.2f, audioTextures[i].getSize().y/2.2f});
    aud.setOrigin({aud.getSize().x/2.f, aud.getSize().y/2.f});
    aud.setPosition({40, 30});
    aud.setTexture(&audioTextures[i], true);
}

bool inProximity(RectangleShape rect, Vector2f mpos, float distance) {
    if(mpos.x > rect.getGlobalBounds().position.x - distance &&
       mpos.x < rect.getGlobalBounds().position.x + rect.getGlobalBounds().size.x + distance &&
       mpos.y > rect.getGlobalBounds().position.y - distance &&
       mpos.y < rect.getGlobalBounds().position.y + rect.getGlobalBounds().size.y + distance)
        return true;
    return false;
}

void setVolume(RectangleShape musicVolumeBack, Vector2f mpos, Music& backgroundMusic) {
    if(mpos.x >= musicVolumeBack.getPosition().x)
        backgroundMusic.setVolume( min((mpos.x - musicVolumeBack.getPosition().x) / (musicVolumeBack.getSize().x/100.f), 100.f) );
    else
        backgroundMusic.setVolume(0);
}