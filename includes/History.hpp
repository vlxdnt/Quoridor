#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Entities.hpp"
#include "UI.hpp"

using namespace sf;

class Move {
public:
    bool isMove = true; // 0 perete, 1 mutare
    Vector2i pos = {0, 0};
    Orientation wallOrientation = Orientation::Horizontal; 
};

class historyMove {
public:
    Move move;
    Text moveText;
    historyMove(const Font& font) : moveText(font){}
};

class History {
public:
    RectangleShape backGround;
    Button undoButton;
    RectangleShape scrollBar;
    RectangleShape scrollBarBackground;
    std::vector< std::vector<historyMove> > moves;
    Text cycleIndex;
    Font hFont;
    int elapsedTurns;
    std::vector<Vector2u> initialpositions;
    float scroll;
    float maxScroll;

    History(const Font& font, int playerCount, Piesa players[4]) : undoButton(font, "undo", {75,40},{15,15}), cycleIndex(font)
    {
        elapsedTurns = 0;
        scroll = 0;
        maxScroll = 0;
        hFont = font;
        cycleIndex.setCharacterSize(22);
        backGround.setSize( {(SCREEN_W - BOARD_SIZE) / 2 - 50, SCREEN_H} );
        backGround.setPosition( {SCREEN_W - backGround.getSize().x, 0} );
        backGround.setFillColor(Color(5, 9, 16));
        scrollBarBackground.setSize({20, SCREEN_H - 200.f});
        scrollBarBackground.setPosition( {SCREEN_W - 10 - scrollBarBackground.getSize().x,
                                         (SCREEN_H - scrollBarBackground.getSize().y) / 2.f} );
        scrollBarBackground.setFillColor(Color::Black);
        moves.resize(playerCount);
        initialpositions.resize(playerCount);

        undoButton.move(backGround.getPosition());

        if(playerCount >= 2) {
            initialpositions.at(0) = Vector2u(players[0].position.x, players[0].position.y);
            initialpositions.at(1) = Vector2u(players[1].position.x, players[1].position.y);
        }
        if (playerCount == 4) {
            initialpositions.at(2) = Vector2u(players[2].position.x, players[2].position.y);
            initialpositions.at(3) = Vector2u(players[3].position.x, players[3].position.y);
        }
    }

    void addMove(int turn, bool isMove, int x, int y, bool wallOrientation, Color color = Color::White)
    {
        elapsedTurns++;
        historyMove mutare(hFont);
        mutare.move.isMove = isMove;
        mutare.move.pos.x = x;
        mutare.move.pos.y = y;
        mutare.move.wallOrientation = wallOrientation ? Orientation::Vertical : Orientation::Horizontal;
        mutare.moveText.setString(moveTranslate(mutare.move));
        mutare.moveText.setCharacterSize(24);
        mutare.moveText.setFillColor(color);
        moves.at(turn).push_back(mutare);
        maxScroll = ( elapsedTurns + static_cast<int>(moves.size()) - 1 ) / static_cast<int>(moves.size()) * 40 - scrollBarBackground.getSize().y;
        this->updateScrollBar();
    }

    std::string moveTranslate(Move mutare)
    {
        std::string mut;
        mut.append(1, char(mutare.pos.x + 97));
        mut.append(1, char(9-mutare.pos.y + 48));
        if(!mutare.isMove) {
            if(mutare.wallOrientation == Orientation::Vertical)
                mut.append(1, 'v');
            else
                mut.append(1, 'h');
        }
        return mut;
    }

    void undo(int& turn, Piesa players[4], RenderTexture& wallsTexture, bool& timerStarted)
    {
        timerStarted = false;
        if(elapsedTurns > 0)
        {
            elapsedTurns--;
            turn = (turn + moves.size() - 1) % moves.size();
            if(moves.at(turn).back().move.isMove)
            {
                moves.at(turn).pop_back();
                bool ok = 0;
                for(int i = static_cast<int>(moves.at(turn).size()) - 1; i >= 0; i--) {
                    if(moves.at(turn).at(i).move.isMove) {
                        players[turn].position.x = moves.at(turn).at(i).move.pos.x;
                        players[turn].position.y = moves.at(turn).at(i).move.pos.y;
                        ok = 1;
                        break;
                    }
                }
                if(!ok) {
                    players[turn].position.x = initialpositions.at(turn).x;
                    players[turn].position.y = initialpositions.at(turn).y;
                }
            }
            else //wall undo
            {
                players[turn].pereti++;
                walls[moves.at(turn).back().move.pos.x][moves.at(turn).back().move.pos.y].hasHorizontal = 0;
                walls[moves.at(turn).back().move.pos.x][moves.at(turn).back().move.pos.y].hasVertical = 0;
                moves.at(turn).pop_back();
                wallsTexture.clear(Color::Transparent);

                // redraw walls
                float drawX, drawY;
                Vector2f Size;
                RectangleShape perete;
                perete.setFillColor(Color(153, 72, 29));
                for(int i = 0; i < elapsedTurns; i++) {
                    if(!moves.at(i%moves.size()).at(i/moves.size()).move.isMove)
                    {
                        if (moves.at(i%moves.size()).at(i/moves.size()).move.wallOrientation == Orientation::Vertical) { /// vertical
                            Size = Vector2f(-WALL_WIDTH, WALL_LENGTH);
                            drawX = CELL_GAP + (moves.at(i%moves.size()).at(i/moves.size()).move.pos.x + 1) * (DIM_CELL + CELL_GAP);
                            drawY = BOARD_SIZE - (moves.at(i%moves.size()).at(i/moves.size()).move.pos.y + 2) * (DIM_CELL + CELL_GAP);
                        }
                        else { /// orizontal
                            Size = Vector2f(WALL_LENGTH, -WALL_WIDTH);
                            drawX = CELL_GAP + moves.at(i%moves.size()).at(i/moves.size()).move.pos.x * (DIM_CELL + CELL_GAP);
                            drawY = BOARD_SIZE - (moves.at(i%moves.size()).at(i/moves.size()).move.pos.y + 1) * (DIM_CELL + CELL_GAP);
                        }
                        perete.setSize(Size);
                        perete.setPosition({ drawX, drawY });

                        wallsTexture.draw(perete);
                    }
                }
            }
        }
        maxScroll = ( elapsedTurns + static_cast<int>(moves.size()) - 1 ) / static_cast<int>(moves.size()) * 40 - scrollBarBackground.getSize().y;
        this->updateScrollBar();
    }

    void updateScrollBar()
    {
        if(scroll > maxScroll && maxScroll >= 0)
            scroll = maxScroll;
        else if(maxScroll < 0)
            scroll = 0;
        
        float den = maxScroll + scrollBarBackground.getSize().y;
        if (den != 0) {
            scrollBar.setSize({scrollBarBackground.getSize().x,
                               scrollBarBackground.getSize().y * scrollBarBackground.getSize().y / den});
        }
        
        if (maxScroll != 0) {
            scrollBar.setPosition({scrollBarBackground.getPosition().x,
                                   scrollBarBackground.getPosition().y + scroll * (scrollBarBackground.getSize().y - scrollBar.getSize().y) / maxScroll});
        }
    }

    void render(RenderWindow& window)
    {
        window.draw(backGround);
        // render moves
        int moveSizeInt = static_cast<int>(moves.size());
        for(int i = 0; i < (elapsedTurns + moveSizeInt - 1) / moveSizeInt; i++)
        {
            cycleIndex.setString(std::to_string(i+1) + ".");
            cycleIndex.setPosition(backGround.getPosition() + Vector2f(30, 100 - scroll + 40 * i));
            window.draw(cycleIndex);
            
            int limit = std::min(elapsedTurns - moveSizeInt * i, moveSizeInt);
            for(int j = 0; j < limit; j++)
            {
                moves.at(j).at(i).moveText.setPosition(cycleIndex.getPosition() + Vector2f(55 * j + 30, 0));
                window.draw(moves.at(j).at(i).moveText);
            }
        }

        undoButton.render(window);
        //scroll bar
        if(maxScroll > 0)
        {
            window.draw(scrollBarBackground);
            window.draw(scrollBar);
        }
    }
};