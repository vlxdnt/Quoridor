#include "GameRender.hpp"

using namespace std;
using namespace sf;

void drawBoard(RenderWindow& window, Texture& cellTexture, Font& font) {
    Vector2u winSize = window.getSize();
    float startX = (winSize.x - BOARD_SIZE) / 2.0f;
    float startY = (winSize.y - BOARD_SIZE) / 2.0f;

    RectangleShape backgroundTabla({ BOARD_SIZE, BOARD_SIZE });
    backgroundTabla.setFillColor(Color(1, 10, 22));
    backgroundTabla.setPosition({ startX, startY });
    window.draw(backgroundTabla);

    RectangleShape cell({ DIM_CELL, DIM_CELL });
    cell.setTexture(&cellTexture);
    cell.setFillColor(Color::White);

    for (int i = 0; i < DIM_TABLA; i++) {
        for (int j = 0; j < DIM_TABLA; j++) {
            float x = startX + CELL_GAP + i * (DIM_CELL + CELL_GAP);
            float y = startY + CELL_GAP + j * (DIM_CELL + CELL_GAP);

            cell.setPosition({ x, y });
            window.draw(cell);
        }
    }

    vector<Text> coords;
    for (int i = 0; i < 9; i++) {
        Text labels(font, string(1, 'a' + i), 24);
        labels.setFillColor(Color::White);

        float posX = startX + BOARD_OFFSETX + (i * (DIM_CELL + CELL_GAP));
        float posY = startY + BOARD_OFFSETY + 8 * (DIM_CELL + CELL_GAP) + DIM_CELL;

        centerTextOrigin(labels);
        labels.setPosition({ posX, posY });
        coords.push_back(labels);
    }

    for (int i = 0; i < 9; i++) {
        Text labels(font, to_string(9 - i), 24);
        labels.setFillColor(Color::White);

        float posX = startX - DIM_CELL / 2.f;
        float posY = BOARD_OFFSETY + (i * (DIM_CELL + CELL_GAP) + (DIM_CELL + CELL_GAP) / 2.f) + DIM_CELL + CELL_GAP * 2.5f;

        centerTextOrigin(labels);
        labels.setPosition({ posX, posY });
        coords.push_back(labels);
    }

    for (auto& label : coords) {
        window.draw(label);
    }
}

void drawPieces(RenderWindow& window, int playerCount, Piesa players[]) {
    Vector2u winSize = window.getSize();
    float startX = (winSize.x - BOARD_SIZE) / 2.0f;
    float startY = (winSize.y - BOARD_SIZE) / 2.0f;

    float raza = 16.f;
    CircleShape piesa({ raza });
    float cellCenter = (DIM_CELL - (raza * 2)) / 2.0f;

    for (int i = 0; i < playerCount; i++) {
        float posX = startX + CELL_GAP + (players[i].position.x * (DIM_CELL + CELL_GAP)) + cellCenter;
        float posY = startY + CELL_GAP + (players[i].position.y * (DIM_CELL + CELL_GAP)) + cellCenter;

        piesa.setFillColor(colorPallete[players[i].indexCuloare]);
        piesa.setPosition({ posX, posY });
        window.draw(piesa);
    }
}

void drawVisitability(int visitability[][DIM_TABLA], RenderWindow& window, float startX, float startY, Piesa players[], int turn) {
    CircleShape circle(DIM_CELL / 3);
    Color moveColor = colorPallete[players[turn].indexCuloare];
    moveColor.a = 75; 
    moveColor = moveColor + Color(50, 50, 50, 0); 
    circle.setFillColor(moveColor);
    circle.setOutlineThickness(2.0f);
    circle.setOutlineColor(Color::White);

    for (int i = 0; i < DIM_TABLA; i++)
        for (int j = 0; j < DIM_TABLA; j++)
            if (visitability[i][j] == 1) {
                circle.setPosition({ CELL_GAP + startX + (DIM_CELL + CELL_GAP) * i + DIM_CELL / 6, CELL_GAP + startY + (DIM_CELL + CELL_GAP) * j + DIM_CELL / 6});
                window.draw(circle);
            }
}

void drawGameUI(RenderWindow& window, int playerCount, int turn, Piesa players[], Font& font) {
    float paddingTop = 100.f;
    float paddingLeft = 20.f;

    for (int i = 0; i < playerCount; i++) {
        string playerTurn = (turn == i) ? "-> " + players[i].name : players[i].name;
        string peretiRamasi = "Pereti ramasi: " + to_string(players[i].pereti);
        Text textPereti(font, peretiRamasi, 24);

        switch (players[i].isBot) {
            case 1: playerTurn.append(" (E)"); break;
            case 2: playerTurn.append(" (M)"); break;
            case 3: playerTurn.append(" (H)"); break;
            default: break;
        }

        Text textNume(font, playerTurn, 42);
        textNume.setFillColor(colorPallete[players[i].indexCuloare]);

        float posX = paddingLeft;
        float posYN = paddingTop + i * paddingTop;
        float posYP = paddingTop + (i + 1) * paddingTop - paddingTop / 2.f;

        textNume.setPosition({ posX,posYN });
        textPereti.setPosition({ posX,posYP });
        window.draw(textNume);
        window.draw(textPereti);
    }

    string pressESC = "Pentru a reveni la meniul principal apasa ESC";
    Text leave(font, pressESC, 20);
    centerTextOrigin(leave);
    float posX = SCREEN_W / 2.f;
    float posY = paddingTop / 2.f;
    leave.setPosition({ posX, posY });
    window.draw(leave);
}

void drawErrorMessage(RenderWindow& window, Font& font) {
    Text errorText(font, "Mutare invalida!", 20);
    errorText.setFillColor(Color::Red);
    centerTextOrigin(errorText);
    errorText.setPosition({ SCREEN_W / 2.f, SCREEN_H * 0.95f});
    window.draw(errorText);
}

void centerTextOrigin(Text& text) {
    FloatRect bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                     bounds.position.y + bounds.size.y / 2.0f });
}

void initializeBackButton(SymbolButton &backButton) {
    VertexArray arrow(PrimitiveType::TriangleFan, 10);
    arrow[0].position = Vector2f(-3, 0);
    arrow[1].position = Vector2f(12, -15);
    arrow[2].position = Vector2f(16, -11);
    arrow[3].position = Vector2f(8, -3);
    arrow[4].position = Vector2f(37, -3);
    arrow[5].position = Vector2f(37, 3);
    arrow[6].position = Vector2f(8, 3);
    arrow[7].position = Vector2f(16, 11);
    arrow[8].position = Vector2f(12, 15);
    arrow[9].position = Vector2f(-3, 0);

    for(int i = 0; i <= 9; i++) arrow[i].color = Color::White;

    backButton.setupArrow(true); 
}