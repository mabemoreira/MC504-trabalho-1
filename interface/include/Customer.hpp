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
    
    int id;
    int state; // 0: descansando, 1: indo para panela, 2: comendo, 3: retornando
    int targetPot; // -1 significa sem panela alvo

public:

    Customer(int index_customer, std::vector<std::string> customerFiles, const sf::Font& font, sf::RenderWindow& window);

    sf::Sprite& getSprite() { return this->sprite; }
    sf::Text& getLabel() { return this->label; }
    sf::Vector2f& getVelocity() { return this->velocity; }
    sf::Texture& getTexture() { return this->texture; }
    int getState() const { return this->state; }
    int getTargetPot() const { return this->targetPot; }
    int getId() const { return this->id; }

    void setSprite(const sf::Sprite& sprite) { this->sprite = sprite; }
    void setLabel(const sf::Text& label) { this->label = label; }
    void setVelocity(const sf::Vector2f& velocity) { this->velocity = velocity; }
    void setState(int state) { this->state = state; }
    void setTargetPot(int targetPot) { this->targetPot = targetPot; }

    void printAttributes() const;

    ~Customer();
};

#endif