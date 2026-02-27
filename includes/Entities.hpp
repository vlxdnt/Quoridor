#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include <string>

class Piesa {
public:
    std::string name;
    sf::Vector2i position;
    int winCondition = 0;
    int indexCuloare = 0;
    int pereti = 0;
    int isBot = 0;
};

class wallMap {
public:
    bool hasHorizontal = false;
    bool hasVertical = false;
};

extern wallMap walls[DIM_TABLA][DIM_TABLA];