#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Entities.hpp"
#include "History.hpp"

using namespace sf;

void runPlayerLogic(const Event& event, float startX, float startY, bool&, int&, int, Piesa* players, int curentVisitability[DIM_TABLA][DIM_TABLA], RenderTexture& wallsTexture, History&, bool&);
void runComputerLogic(Piesa[], int, int&, int curentVisitability[][DIM_TABLA], RenderTexture& wallsTexture, bool&, History&, bool&);
bool checkWallCollision(int x, int y, int direction);
bool isMoveValid(int[][DIM_TABLA], Vector2i target);
void computeVisitable(int[][DIM_TABLA], Piesa[], int, int);
void resetGame(Piesa[], int, int&, bool&, RenderTexture& wallsTexture, int[][DIM_TABLA]);
bool hasWon(Piesa, int, int);
bool isPathClear(Piesa, int, int);
bool placeWall(int x, int y, int orientation, RenderTexture&, Piesa* playerCurent, Piesa players[], int);
void initializePlayers(Piesa[]);
void initializeColorSelector(selectorButtons&, Font&);
void initializeAiSelector(selectorButtons &AI, Font &font);
void audioLevelSprite(RectangleShape&, RectangleShape&, Texture[], Music&);
bool inProximity(RectangleShape, Vector2f, float);
void setVolume(RectangleShape, Vector2f, Music&);
