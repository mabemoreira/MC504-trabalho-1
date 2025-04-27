#ifndef SFML_UTILS_HPP
#define SFML_UTILS_HPP

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
#include "../include/Customer.hpp"
#include "../include/Chef.hpp"

// Network
void receiveSignals(std::map<std::string, int> &signals, std::vector<Customer> &customers, std::vector<Chef> &chefs, sf::Clock &clock);

// Fontes
sf::Font loadFont(const std::string& fontPath);

// Customers
std::vector<std::string> getCustomerFiles();

// Atualizar Sprites
bool moveTowards(sf::Sprite &sprite, sf::Vector2f targetPos, float speed, float deltaTime);
bool moveCostumerToPot(sf::Sprite &costumerSprite, sf::Text &costumerLabel, sf::Sprite &potSprite, float costumerVelocity, float deltaTime, int costumerIndex);
bool watingToEat(sf::Sprite &costumerSprite, sf::Sprite &potSprite, float costumerVelocity, float deltaTime);
bool moveCostumerAfterEat(sf::Sprite &costumerSprite, sf::Text &costumerLabel, sf::Sprite &foodSprite, float velocity, float deltaTime, int costumerIndex);
bool moveChefToPot(sf::Sprite &chefSprite, sf::Text &chefLabel, sf::Texture &activeChefTexture, sf::Sprite &potSprite, float chefVelocity, float deltaTime, int chefIndex);
bool returnChefToRest(sf::Sprite &chefSprite, sf::Text &chefLabel, sf::Texture &returningChef, sf::Texture &sleepingChef, float chefVelocity, float deltaTime, int chefIndex, sf::Vector2f inicialPos);


#endif