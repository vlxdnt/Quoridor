#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameLogic.hpp"
#include "GameRender.hpp"
#include "Menus.hpp"

wallMap walls[DIM_TABLA][DIM_TABLA];

int main()
{
    ContextSettings settings;
    settings.antiAliasingLevel = 8; 
    RenderWindow window(VideoMode({ SCREEN_W, SCREEN_H }), "Quoridor", Style::Titlebar | Style::Close, State::Windowed, settings);
    window.setVerticalSyncEnabled(true); 

    Font font("assets/Minecraft.ttf");
    Texture cellTexture("assets/cell.png");
    Texture audioTextures[5];
    audioTextures[0].loadFromFile("assets/audio_off.png");
    audioTextures[1].loadFromFile("assets/audio_1.png");
    audioTextures[2].loadFromFile("assets/audio_2.png");
    audioTextures[3].loadFromFile("assets/audio_3.png");
    audioTextures[4].loadFromFile("assets/audio_4.png");

    Music backgroundMusic("assets/background.wav");
    backgroundMusic.setLooping(true);
    backgroundMusic.setVolume(30.f);
    backgroundMusic.play();

    SymbolButton backButton(font, {50, 50}, {5,5});
    initializeBackButton(backButton);

    int playerCount = 2;
    int lastWon = -1;

    Menu currentMenu = Menu::MainMenu;
    Menu prevMenu = Menu::MainMenu;

    Piesa players[4];
    initializePlayers(players);

    while (window.isOpen()) {
        switch (currentMenu) {
        case Menu::MainMenu:
            mainMenu(currentMenu, window, backgroundMusic, font, audioTextures);
            prevMenu = Menu::MainMenu;
            break;

        case Menu::Game:
            Game(currentMenu, playerCount, players, lastWon, window, font, cellTexture);
            break;

        case Menu::EndScreen:
            endScreen(currentMenu, playerCount, players, lastWon, window, font);
            break;

        case Menu::Customization:
            customizationScreen(currentMenu, backButton, prevMenu, playerCount, players, window, font);
            break;

        case Menu::PlayerSelection:
            selectPlayerCount(currentMenu, backButton, playerCount, window, font);
            prevMenu = Menu::PlayerSelection;
            break;

        case Menu::Rules:
            ruleScreen(currentMenu, backButton, window, font);
            break;

        default:
            mainMenu(currentMenu, window, backgroundMusic, font, audioTextures);
        }
    }
    return 0;
}