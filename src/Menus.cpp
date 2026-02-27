#include "Menus.hpp"
#include "GameLogic.hpp"
#include "GameRender.hpp"

using namespace sf;
using namespace std;

void mainMenu(Menu& currentMenu, RenderWindow& window, Music& backgroundMusic, Font& font, Texture audioTextures[]) {
    Text title(font, "Quoridor", 128);
    centerTextOrigin(title);

    title.setPosition({ SCREEN_W / 2.f, SCREEN_H / 6.5f });
    title.setOutlineThickness(4);
    title.setOutlineColor(Color(159, 87, 54));

    RectangleShape audioSprite;
    RectangleShape musicVolumeBack({200,10});
    musicVolumeBack.setFillColor(Color::Black);
    musicVolumeBack.setPosition({100,25});
    RectangleShape musicVolume;
    musicVolume.setFillColor(Color::White);
    musicVolume.setPosition({100,25});
    bool audioWasHeld = false;
    audioLevelSprite(audioSprite, musicVolume, audioTextures, backgroundMusic);

    Button playButton(font, "Joaca", { 300, 100 });
    playButton.setPosition({ SCREEN_W / 2.f - playButton.getSize().x / 2.f, SCREEN_H / 3.5f });

    Button customizationButton(font, "Customizare", { 300, 100 });
    customizationButton.setPosition({ SCREEN_W / 2.f - customizationButton.getSize().x / 2.f, SCREEN_H / 2.25f });

    Button rulesButton(font, "Reguli", { 300, 100 });
    rulesButton.setPosition({ SCREEN_W / 2.f - rulesButton.getSize().x / 2.f, SCREEN_H * 2 / 3.3f });

    Button quitButton(font, "Iesire", { 300, 100 });
    quitButton.setPosition({ SCREEN_W / 2.f - quitButton.getSize().x / 2.f, SCREEN_H * 2 / 2.6f });

    while (window.isOpen() && currentMenu == Menu::MainMenu) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mpos(mouseEvent->position);
                    if (playButton.contains(mpos)) currentMenu = Menu::PlayerSelection; 
                    if (customizationButton.contains(mpos)) currentMenu = Menu::Customization; 
                    if (rulesButton.contains(mpos)) currentMenu = Menu::Rules; 
                    if (quitButton.contains(mpos)) window.close();
                    if (inProximity(musicVolumeBack, mpos, 20)) {
                        audioWasHeld = true;
                        setVolume(musicVolumeBack, mpos, backgroundMusic);
                        audioLevelSprite(audioSprite, musicVolume, audioTextures, backgroundMusic);
                    }
                }
            }

            if (const auto* moveEvent = event->getIf<Event::MouseMoved>()) {
                Vector2f mpos(moveEvent->position);
                if(!inProximity(musicVolumeBack, mpos, 20))
                    audioWasHeld = false;
                else if (audioWasHeld) {
                    setVolume(musicVolumeBack, mpos, backgroundMusic);
                    audioLevelSprite(audioSprite, musicVolume, audioTextures, backgroundMusic);
                }
            }

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonReleased>()){
                if (mouseEvent->button == Mouse::Button::Left) audioWasHeld = false;
            }

            if (event->is <Event::KeyPressed>())
                if (Keyboard::isKeyPressed(Keyboard::Scancode::Escape)) window.close();
        }
        window.clear(background);

        window.draw(title);
        playButton.render(window);
        customizationButton.render(window);
        rulesButton.render(window);
        quitButton.render(window);
        window.draw(audioSprite);
        window.draw(musicVolumeBack);
        window.draw(musicVolume);

        window.display();
    }
}

void Game(Menu& currentMenu, int playerCount, Piesa players[], int& lastWon, RenderWindow& window, Font& font, Texture& cellTexture) {
    RenderTexture wallsTexture({ BOARD_SIZE, BOARD_SIZE });
    wallsTexture.clear(Color::Transparent);

    RectangleShape wallObj({ (float)BOARD_SIZE, (float)BOARD_SIZE });
    wallObj.setTexture(&wallsTexture.getTexture());

    float startX = (window.getSize().x - BOARD_SIZE) / 2.0f;
    float startY = (window.getSize().y - BOARD_SIZE) / 2.0f;
    wallObj.setPosition({ startX,startY });

    bool showErrorMessage = false;
    int turn = 0;
    bool isGameRunning = true;
	bool timerStarted = false;
    int curentVisitability[DIM_TABLA][DIM_TABLA];

    resetGame(players, playerCount, turn, isGameRunning, wallsTexture, curentVisitability);

    History history(font, playerCount, players);
    bool scrollBarHeld = false;
    float scrollClickMouseY;
    float prevScrollValue;

    while (window.isOpen() && currentMenu == Menu::Game) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();

            if (event->is <Event::KeyPressed>())
                if (Keyboard::isKeyPressed(Keyboard::Scancode::Escape)) {
                    currentMenu = Menu::MainMenu; 
                    break;
                }
            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>())
                if (mouseEvent->button == Mouse::Button::Left) {
                    if(history.undoButton.contains(Vector2f(mouseEvent->position))) {
                        history.undo(turn, players, wallsTexture, timerStarted);
                        computeVisitable(curentVisitability, players, playerCount, turn);
                    }
                    if (inProximity(history.scrollBar, Vector2f(mouseEvent->position), 0)) {
                        scrollBarHeld = true;
                        scrollClickMouseY = mouseEvent->position.y;
                        prevScrollValue = history.scroll;
                    }
                }

            if (const auto* moveEvent = event->getIf<Event::MouseMoved>()) {
                if (scrollBarHeld) {
                    history.scroll = std::max(0.f, std::min(prevScrollValue - (scrollClickMouseY - moveEvent->position.y), history.maxScroll));
                    history.updateScrollBar();
                }
            }

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonReleased>())
                if (mouseEvent->button == Mouse::Button::Left) scrollBarHeld = false;

            if (const auto* mouseEvent = event->getIf<Event::MouseWheelScrolled>())
                if(inProximity(history.backGround, Vector2f(mouseEvent->position), 0)) {
                    history.scroll = std::max(0.f, std::min(history.scroll - 20 * mouseEvent->delta, history.maxScroll));
                    history.updateScrollBar();
                }

            if (isGameRunning && players[turn].isBot == 0)
                runPlayerLogic(*event, startX, startY, isGameRunning, turn, playerCount, players, curentVisitability, wallsTexture, history, showErrorMessage);
        }

        if (isGameRunning && players[turn].isBot != 0)
            runComputerLogic(players, playerCount, turn, curentVisitability, wallsTexture, isGameRunning, history, timerStarted);

        window.clear(background);

        if (!isGameRunning) {
            currentMenu = Menu::EndScreen;
            lastWon = turn;
        }
        drawBoard(window, cellTexture, font);
        window.draw(wallObj);
        drawPieces(window, playerCount, players);
        if (!players[turn].isBot)
            drawVisitability(curentVisitability, window, startX, startY, players, turn);
        drawGameUI(window, playerCount, turn, players, font);
        history.render(window);
        if (showErrorMessage)
            drawErrorMessage(window,font);
        window.display();
    }
}

void endScreen(Menu& currentMenu, int playerCount, Piesa players[], int lastWon, RenderWindow& window, Font& font) {
    Button replayButton(font, "Joaca din nou", { 250, 50 });
    replayButton.setPosition({ SCREEN_W / 2 - replayButton.getSize().x / 2, SCREEN_H / 2 });
    Button menuButton(font, "Inapoi la meniu", { 250, 50 });
    menuButton.setPosition({ SCREEN_W / 2 - menuButton.getSize().x / 2, SCREEN_H / 2 + menuButton.getSize().y * 1.5f });
    Button quitButton(font, "Iesire", { 250, 50 });
    quitButton.setPosition({ SCREEN_W / 2 - quitButton.getSize().x / 2, SCREEN_H / 2 + quitButton.getSize().y * 3.f});

    Text textEnd(font, "Jocul s-a terminat.", 64);
    string playerWin = (lastWon >= 0 && lastWon < playerCount) ? "Jucatorul " + players[lastWon].name + " a castigat!" : "";
    Text textWin(font, playerWin, 64);
    centerTextOrigin(textEnd);
    centerTextOrigin(textWin);
    textEnd.setPosition({ SCREEN_W / 2,SCREEN_H / 2 - 176 });
    textWin.setPosition({ SCREEN_W / 2,SCREEN_H / 2 - 96 });

    while (window.isOpen() && currentMenu == Menu::EndScreen) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mpos(mouseEvent->position);
                    if (replayButton.contains(mpos)) currentMenu = Menu::Game; 
                    if (menuButton.contains(mpos)) currentMenu = Menu::MainMenu;
                    if (quitButton.contains(mpos)) window.close();
                }
            }

            if (event->is <Event::KeyPressed>())
                if (Keyboard::isKeyPressed(Keyboard::Scancode::Escape)) window.close();
        }
        window.clear(background);
        window.draw(textEnd);
        window.draw(textWin);
        menuButton.render(window);
        replayButton.render(window);
        quitButton.render(window);
        window.display();
    }
}

void customizationScreen(Menu& currentMenu, SymbolButton& backButton, Menu prevMenu, int playercount, Piesa players[], RenderWindow& window, Font& font) {
    int playerToEdit = 0;
    initializePlayers(players);

    if(prevMenu != Menu::PlayerSelection) playercount = 4;

    Button playButton(font, "Joaca", {140,70}, Vector2f(window.getSize()) - Vector2f(150,80));

    RectangleShape textField({300, 75});
    textField.setFillColor(Color(2, 4, 8));
    textField.setPosition(Vector2f(window.getSize())/2.f - textField.getSize()/2.f);

    Button setButton(font, "Seteaza", {120,40});
    setButton.setPosition(textField.getPosition() + textField.getSize() + Vector2f(20, -(textField.getSize().y/2.f + setButton.getSize().y/2.f)));

    Text field(font);
    Text currentPlayer(font, players[playerToEdit].name);
    currentPlayer.setFillColor(colorPallete[players[playerToEdit].indexCuloare]);
    centerTextOrigin(currentPlayer);
    currentPlayer.setPosition({window.getSize().x/2.f, 60});

    NavigateButtons selectPlayer(font, currentPlayer.getPosition(), 250);

    selectorButtons selectColor(font, "Culoare", {window.getSize().x / 2.f - textField.getSize().x / 2.f, window.getSize().y / 2.f + textField.getSize().y});
    initializeColorSelector(selectColor, font);
    selectColor.updateSelected(players[playerToEdit].indexCuloare);

    selectorButtons selectAi(font, "Strategie bot", selectColor.position + Vector2f(0, 60));
    initializeAiSelector(selectAi, font);
    selectAi.updateSelected(players[playerToEdit].isBot);

    while (window.isOpen() && currentMenu == Menu::Customization) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mpos(mouseEvent->position);
                    if(backButton.contains(mpos)) currentMenu = prevMenu;
                    if(playButton.contains(mpos) && prevMenu == Menu::PlayerSelection) currentMenu = Menu::Game; 
                    if(setButton.contains(mpos)) {
                        bool ok = 1;
                        for(int i = 0; i < playercount; i++)
                            if(field.getString() == players[i].name && i != playerToEdit) ok = 0;
                        if (!field.getString().isEmpty() && ok) {
                            players[playerToEdit].name = field.getString();
                            currentPlayer.setString(players[playerToEdit].name);
                            centerTextOrigin(currentPlayer);
                            currentPlayer.setPosition({window.getSize().x/2.f, 60});
                        }
                    }
                    if(selectPlayer.getClicked(mpos) == -1) {
                        if(prevMenu == Menu::PlayerSelection && playercount == 2) playerToEdit = (playerToEdit + 1) % 2;
                        else playerToEdit = (playerToEdit + 3) % 4;
                        selectColor.updateSelected(players[playerToEdit].indexCuloare);
                        selectAi.updateSelected(players[playerToEdit].isBot);
                        currentPlayer.setFillColor(colorPallete[players[playerToEdit].indexCuloare]);
                        currentPlayer.setString(players[playerToEdit].name);
                        centerTextOrigin(currentPlayer);
                        currentPlayer.setPosition({window.getSize().x/2.f, 60});
                        field.setString("");
                    }
                    if(selectPlayer.getClicked(mpos) == 1) {
                        if(prevMenu == Menu::PlayerSelection && playercount == 2) playerToEdit = (playerToEdit + 1) % 2;
                        else playerToEdit = (playerToEdit + 1) % 4;
                        selectColor.updateSelected(players[playerToEdit].indexCuloare);
                        selectAi.updateSelected(players[playerToEdit].isBot);
                        currentPlayer.setFillColor(colorPallete[players[playerToEdit].indexCuloare]);
                        currentPlayer.setString(players[playerToEdit].name);
                        centerTextOrigin(currentPlayer);
                        currentPlayer.setPosition({window.getSize().x/2.f, 60});
                        field.setString("");
                    }
                    if(selectColor.contains(mpos)) {
                        int ind = selectColor.contains(mpos) - 1;
                        bool ok = 1;
                        for(int i = 0; i < playercount; i++)
                            if(ind == players[i].indexCuloare && i != playerToEdit) ok = 0;
                        if(ok) players[playerToEdit].indexCuloare = ind;
                        currentPlayer.setFillColor(colorPallete[players[playerToEdit].indexCuloare]);
                    }
                    if(selectAi.contains(mpos)) {
                        players[playerToEdit].isBot = selectAi.contains(mpos) - 1;
                    }
                }
            }

            if (const auto* moveEvent = event->getIf<Event::MouseMoved>()) {
                Vector2f mpos(moveEvent->position);
                
                if (selectPlayer.leftContains(mpos))
                    selectPlayer.setLeftFillColor(Color(40,60,80));
                else
                    selectPlayer.setLeftFillColor(background);
                    
                if (selectPlayer.rightContains(mpos))
                    selectPlayer.setRightFillColor(Color(40,60,80));
                else
                    selectPlayer.setRightFillColor(background);
                    
                if (setButton.contains(mpos))
                    setButton.setFillColor(Color(40,60,80));
                else
                    setButton.setFillColor(background);
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if(field.getString().getSize() < 9) {
                    for(int i = 0; i < 26; i++)
                        if (keyPressed->scancode == static_cast<Keyboard::Scancode>(i)) { 
                            sf::String desc = sf::Keyboard::getDescription(static_cast<Keyboard::Scancode>(i));
                            if (!desc.isEmpty()) {
                                char c = desc[0];
                                field.setString(field.getString() + static_cast<char>(keyPressed->shift ? toupper(c) : tolower(c)));
                                centerTextOrigin(field);
                                field.setPosition(Vector2f(window.getSize())/2.f);
                            }
                        }

                    for(int i = 26; i < 36; i++)
                        if (keyPressed->scancode == static_cast<Keyboard::Scancode>(i)) {
                            field.setString(field.getString() + Keyboard::getDescription(static_cast<Keyboard::Scancode>(i)));
                            centerTextOrigin(field);
                            field.setPosition(Vector2f(window.getSize())/2.f);
                        }
                }

                if(!field.getString().isEmpty()) {
                    if(keyPressed->scancode == Keyboard::Scancode::Backspace) {
                        String fieldString = field.getString();
                        fieldString.erase(fieldString.getSize() - 1, 1);
                        field.setString(fieldString);
                        centerTextOrigin(field);
                        field.setPosition(Vector2f(window.getSize())/2.f);
                    }

                    if(keyPressed->scancode == Keyboard::Scancode::Enter) {
                        bool ok = 1;
                        for(int i = 0; i < playercount; i++)
                            if(field.getString() == players[i].name && i != playerToEdit) ok = 0;
                        if(ok) {
                            players[playerToEdit].name = field.getString();
                            currentPlayer.setString(players[playerToEdit].name);
                            centerTextOrigin(currentPlayer);
                            currentPlayer.setPosition({window.getSize().x/2.f, 60});
                        }
                    }
                }
            }
        }

        window.clear(background);
        backButton.render(window);
        window.draw(textField);
        window.draw(field);
        window.draw(currentPlayer);
        setButton.render(window);
        selectPlayer.render(window);
        selectColor.render(window);
        selectAi.render(window);
        if(prevMenu == Menu::PlayerSelection)
            playButton.render(window);
        window.display();
    }
}

void selectPlayerCount(Menu& currentMenu, SymbolButton& backButton, int& playerCount, RenderWindow& window, Font& font) {
    Text text(font, "Selecteaza numarul de jucatori:", 64);
    centerTextOrigin(text);
    text.setPosition({ SCREEN_W / 2.f, SCREEN_H / 3.f });
    Button twoPlayers(font, "2 Jucatori", { 300, 100 });
    Button fourPlayers(font, "4 Jucatori", { 300, 100 });

    twoPlayers.setPosition({ SCREEN_W / 2.f - twoPlayers.getSize().x / 2.f, SCREEN_H / 2.f });
    fourPlayers.setPosition({ SCREEN_W / 2.f - fourPlayers.getSize().x / 2.f, SCREEN_H / 2.f + fourPlayers.getSize().y * 1.5f });

    twoPlayers.getLabel().setCharacterSize(48);
    fourPlayers.getLabel().setCharacterSize(48);
    centerTextOrigin(twoPlayers.getLabel());
    centerTextOrigin(fourPlayers.getLabel());

    while (window.isOpen() && currentMenu == Menu::PlayerSelection) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();

            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mpos(mouseEvent->position);
                    if (twoPlayers.contains(mpos)) {
                        playerCount = 2;
                        currentMenu = Menu::Customization; 
                    }
                    if (fourPlayers.contains(mpos)) {
                        playerCount = 4;
                        currentMenu = Menu::Customization; 
                    }
                    if (backButton.contains(mpos)) {
                        currentMenu = Menu::MainMenu; 
                    }
                }
            }
        }
        window.clear(background);
        backButton.render(window);
        window.draw(text);
        twoPlayers.render(window);
        fourPlayers.render(window);
        window.display();
    }
}

void ruleScreen(Menu& currentMenu, SymbolButton& backButton, RenderWindow& window, Font& font) {
    Text header(font, "Reguli", 64);
    centerTextOrigin(header);
    header.setPosition({ SCREEN_W / 2.f, SCREEN_H * 0.1f });

    Text rules(font);
    rules.setCharacterSize(32);
    rules.setPosition({ 0, SCREEN_H * 0.2f });
    rules.setString(R"(
        1. Te poti misca apasand click stanga pe cerculetele mai mici de
        aceeasi culoare ca piesa ta;

        2. Poti pune pereti apasand click dreapta pe spatiul liber dintre celule;

        3. Pentru a castiga trebuie sa ajungi pe linia/coloana din capatul opus
        mai repede decat adversarul/adversarii;

        Mult Noroc!
    )");

    while (window.isOpen() && currentMenu == Menu::Rules) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();

            if (const auto *mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mpos(mouseEvent->position);
                    if (backButton.contains(mpos)) currentMenu = Menu::MainMenu;
                }
            }
        }
        window.clear(background);
        window.draw(header);
        window.draw(rules);
        backButton.render(window);
        window.display();
    }
}