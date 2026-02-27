#pragma once

#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Entities.hpp"
#include "UI.hpp"
#include "History.hpp"

void mainMenu(Menu&, RenderWindow&, Music&, Font&, Texture[]);
void Game(Menu&, int, Piesa[], int&, RenderWindow&, Font&, Texture&);
void endScreen(Menu&, int, Piesa[], int, RenderWindow&, Font&);
void customizationScreen(Menu&, SymbolButton&, Menu, int, Piesa[], RenderWindow&, Font&);
void selectPlayerCount(Menu&, SymbolButton&, int& , RenderWindow&, Font&);
void ruleScreen(Menu&, SymbolButton&, RenderWindow&, Font&);