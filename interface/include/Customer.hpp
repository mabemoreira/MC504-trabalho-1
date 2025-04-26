#ifndef CUSTOMER_HPP
#define CUSTOMER_HPP

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

class Customer {
private:
    std::string spriteFile; // Caminho do arquivo de sprite
    sf::Texture texture; // Textura do sprite
    sf::Sprite sprite;
    sf::Text label; // Label contendo nome do cliente
    sf::Vector2f velocity;
    
    bool moving;
    bool eating;
    bool leaving;
    int targetPot; // -1 significa sem panela alvo
    float eatingTimer; // Tempo restante comendo

public:

    Customer(int index_customer, std::vector<std::string> customerFiles, const sf::Font& font, sf::RenderWindow& window);

    sf::Sprite& getSprite() { return this->sprite; }
    sf::Text& getLabel() { return this->label; }
    sf::Vector2f& getVelocity() { return this->velocity; }
    sf::Texture& getTexture() { return this->texture; }
    bool isMoving() const { return moving; }
    bool isEating() const { return eating; }
    bool isLeaving() const { return leaving; }
    int getTargetPot() const { return this->targetPot; }
    float getEatingTimer() const { return this->eatingTimer; }

    void setSprite(const sf::Sprite& sprite) { this->sprite = sprite; }
    void setLabel(const sf::Text& label) { this->label = label; }
    void setVelocity(const sf::Vector2f& velocity) { this->velocity = velocity; }
    void setMoving(bool moving) { this->moving = moving; }
    void setEating(bool eating) { this->eating = eating; }
    void setLeaving(bool leaving) { this->leaving = leaving; }
    void setTargetPot(int targetPot) { this->targetPot = targetPot; }
    void setEatingTimer(float eatingTimer) { this->eatingTimer = eatingTimer; }

    void decreaseEatingTimer(float deltaTime) { this->eatingTimer -= deltaTime; }

    void printAttributes() const;

    ~Customer();
};

#endif