#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

constexpr int DIM_TABLA = 9; /// celule
constexpr int CELL_GAP = 8; /// px
constexpr int DIM_CELL = 48; /// px
constexpr int BOARD_OFFSETX = 36;
constexpr int BOARD_OFFSETY = 36;
constexpr int BOARD_SIZE = ((DIM_TABLA * DIM_CELL) + ((DIM_TABLA + 1) * CELL_GAP)); /// dimensiunea tablei in pixeli
constexpr int WALL_LENGTH = (2 * DIM_CELL + CELL_GAP);
constexpr int WALL_WIDTH = CELL_GAP;
constexpr int SCREEN_H = 720;
constexpr int SCREEN_W = 1280;
constexpr int dx[] = { 0, 1, 0, -1 }, dy[] = { -1, 0, 1, 0 }; /// N, E, S, V
constexpr int numarPereti = 20;
const Color background = Color(5, 18, 32);
const Color colorPallete[] = {Color::Blue, Color::Red, Color::Yellow, Color::Green, Color(255,127,0), Color::Cyan, Color::Magenta, Color::White};

enum class Menu {
    MainMenu, // indexare de la 0
    Game,
    EndScreen,
    Customization,
    PlayerSelection,
    Rules
};

enum class Orientation {
    Horizontal,
    Vertical
};