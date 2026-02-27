#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Entities.hpp"
#include "History.hpp"
using namespace sf;

void drawBoard(RenderWindow&, Texture&, Font&);
void drawPieces(RenderWindow&, int, Piesa[]);
void drawVisitability(int[][DIM_TABLA], RenderWindow& window, float startX, float startY, Piesa[], int);
void drawGameUI(RenderWindow&, int, int, Piesa[], Font&);
void drawErrorMessage(RenderWindow&, Font&);
void centerTextOrigin(Text&);
void initializeBackButton(SymbolButton&);