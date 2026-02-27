#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Entities.hpp"

using namespace sf;

class Button {
protected:
    RectangleShape corp;
    Text label;

public:
    Button(const Font& font, std::string text, Vector2f size, Vector2f position = { 0,0 }, Color outline = Color::White) 
    : label(font), corp(size)
    { 
        corp.setFillColor(Color(159, 87, 54));
        corp.setOutlineThickness(2);
        corp.setOutlineColor(outline);
        corp.setPosition(position);

        label.setString(text);
        label.setCharacterSize(28);
        label.setFillColor(Color::White);
        
        FloatRect bounds = label.getLocalBounds();
        label.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                          bounds.position.y + bounds.size.y / 2.0f });

        label.setPosition(position + size / 2.f);
    }

    bool contains(Vector2f pos) { return corp.getGlobalBounds().contains(pos); }
    Vector2f getSize() { return corp.getSize(); }
    Vector2f getPosition() const { return corp.getPosition(); }
    void setFillColor(Color color) { corp.setFillColor(color); }
    const RectangleShape& getCorp() const { return corp; }
    Text& getLabel() { return label; }

    void setSize(Vector2f size) {
        corp.setSize(size);
        label.setOrigin({ label.getLocalBounds().position.x + label.getLocalBounds().size.x / 2.0f,
                          label.getLocalBounds().position.y + label.getLocalBounds().size.y / 2.0f });
        label.setPosition(corp.getPosition() + size / 2.f);
    }

    void setString(std::string text) {
        label.setString(text);
        FloatRect bounds = label.getLocalBounds();
        label.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                          bounds.position.y + bounds.size.y / 2.0f });
        label.setPosition(corp.getPosition() + corp.getSize() / 2.f);
    }

    virtual void setPosition(Vector2f pos) {
        corp.setPosition(pos);
        label.setPosition(pos + corp.getSize() / 2.f);
    }

    virtual void move(Vector2f offset) {
        corp.move(offset);
        label.move(offset);
    }

    virtual void render(RenderWindow& window) {
        window.draw(corp);
        window.draw(label);
    }

    virtual ~Button() = default;
};

class SymbolButton : public Button {
public:
    VertexArray symbol;
    Transform symbolTransform;
    
public:
    SymbolButton(const Font& font, Vector2f size = {0, 0}, Vector2f position = {0, 0}) 
    : Button(font, "", size, position) {
    }

    void setupArrow(bool isLeft) {
        symbol.setPrimitiveType(PrimitiveType::LineStrip);
        symbol.resize(3);

        if (isLeft) {
            symbol[0].position = {10, -10};
            symbol[1].position = {0, 0};
            symbol[2].position = {10, 10};
        } else {
            symbol[0].position = {-10, -10};
            symbol[1].position = {0, 0};
            symbol[2].position = {-10, 10};
        }

        for (int i = 0; i < 3; i++) {
            symbol[i].color = Color::White;
        }
        
        setPosition(getPosition()); 
    }       

    void setPosition(Vector2f pos) override {
        Button::setPosition(pos);
        symbolTransform = Transform::Identity;
        symbolTransform.translate(pos + getSize() / 2.f);
    }

    void move(Vector2f offset) override {
        Button::move(offset); 
        symbolTransform.translate(offset);
    }

    void render(RenderWindow& window) override {
        window.draw(corp);
        window.draw(symbol, symbolTransform);
    }
};

class NavigateButtons {
private:
    SymbolButton leftBtn;
    SymbolButton rightBtn;

public:
    NavigateButtons(const Font& font, Vector2f pos, float gap) 
    : leftBtn(font, {25,25}), rightBtn(font, {25,25})
    {
        leftBtn.setPosition({ pos.x - gap, pos.y });
        rightBtn.setPosition({ pos.x + gap, pos.y });
        leftBtn.setupArrow(true);
        rightBtn.setupArrow(false);
    }

    int getClicked(Vector2f mousePos) {
        if (leftBtn.contains(mousePos)) return -1;
        if (rightBtn.contains(mousePos)) return 1;
        return 0;
    }

    bool leftContains(Vector2f pos) { return leftBtn.contains(pos); }
    bool rightContains(Vector2f pos) { return rightBtn.contains(pos); }
    
    void setLeftFillColor(Color color) { leftBtn.setFillColor(color); }
    void setRightFillColor(Color color) { rightBtn.setFillColor(color); }

    void render(RenderWindow& window) {
        leftBtn.render(window);
        rightBtn.render(window);
    }
};

class selectorButtons {
public:
    std::vector<Button> options;
    int optionsLength = 0; // in pixels
    Text category;
    Vector2f position;
    RectangleShape selectedRect;
    unsigned int selected = 0;
    int verticalOffset = 0; 

    selectorButtons(const Font& font, std::string text, Vector2f pos = { 0,0 }) : category(font) {
        position = pos;
        category.setString(text + ":");
        category.setFillColor(Color::White);
        category.setOrigin({category.getLocalBounds().size.x, category.getLocalBounds().size.y});
        category.setPosition(position);
        selectedRect.setFillColor(Color(255,255,255,40));
        selectedRect.setOutlineColor(Color::White);
        selectedRect.setOutlineThickness(3);
    }

    void addOption(Button option) {
        option.setPosition(position + Vector2f(optionsLength + 20, -(option.getSize().y / 2.f + verticalOffset)));
        optionsLength += 20 + option.getSize().x;
        options.push_back(option);
    }

    int contains(Vector2f pos) {
        for(size_t i = 0; i < options.size(); i++) {
            if(options.at(i).contains(pos)) {
                updateSelected(static_cast<int>(i));
                return selected;
            }
        }
        return 0; 
    }

    void updateSelected(int i) {
        if(i < 0 || i >= static_cast<int>(options.size()))
            return;
        selected = i + 1;
        selectedRect.setSize(options.at(i).getSize());
        selectedRect.setPosition(options.at(i).getPosition()); 
    }

    void render(RenderWindow &window) {
        window.draw(category);
        
        for(const Button& bu : options)
            window.draw(bu.getCorp());
            
        if(selected > 0 && selected <= options.size())
            window.draw(selectedRect);
            
        for(Button& bu : options)
            window.draw(bu.getLabel());
    }
};