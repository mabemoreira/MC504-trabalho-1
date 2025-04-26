#ifndef CHEF_HPP
#define CHEF_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>
#include <iostream>
#include <thread>
#include <map>
#include <cmath>

namespace fs = std::filesystem;
using namespace std;

class Chef {
private:
    sf::Sprite sprite;
    sf::Text label;
    sf::Vector2f restPosition;
    int id;
    int state; // 0: descansando, 1: indo para panela, 2: cozinhando, 3: retornando
    int targetPot; // -1 significa sem panela alvo
    float cookingTimer; // Tempo restante cozinhando
    float restTimer; // Tempo restante descansando


public:
    Chef(int index_chef, sf::Font& font);
    
    sf::Sprite& getSprite() { return this->sprite; }
    sf::Text& getLabel() { return this->label; }
    sf::Vector2f& getRestPosition() { return this->restPosition; }
    int getId() const { return this->id; }
    int getState() const { return this->state; }
    int getTargetPot() const { return this->targetPot; }
    float getCookingTimer() const { return this->cookingTimer; }
    float getRestTimer() const { return this->restTimer; }
    
    void setState(int state) { this->state = state; }
    void setLabel(const sf::Text& label) { this->label = label; }
    void setSprite(const sf::Sprite& sprite) { this->sprite = sprite; }
    void setRestPosition(const sf::Vector2f& position) { this->restPosition = position; }
    void setRestTimer(float restTimer) { this->restTimer = restTimer; }
    void setTargetPot(int targetPot) { this->targetPot = targetPot; }
    void setCookingTimer(float cookingTimer) { this->cookingTimer = cookingTimer; }
    

    void decreaseRestTimer(float deltaTime) { this->restTimer -= deltaTime; }
    void decreaseCookingTimer(float deltaTime) { this->cookingTimer -= deltaTime; }
    // ~Chef();

};

#endif