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
#include "../include/Customer.hpp"

namespace fs = std::filesystem;
using namespace std;

Customer::Customer(int index_customer, std::vector<std::string> customerFiles, const sf::Font& font, sf::RenderWindow& window)
    : state(0), targetPot(-1), id(index_customer)
{
    std::srand(std::time(nullptr)); // Inicializa o gerador de números aleatórios

    int randomIndex = std::rand() % customerFiles.size();
    if (!this->texture.loadFromFile(customerFiles[randomIndex])) {
        std::cerr << "Erro ao carregar textura: " << customerFiles[randomIndex] << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Configura o sprite
    this->sprite.setTexture(this->texture);
    this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 2, this->sprite.getGlobalBounds().height / 2);

    // Posição aleatória na tela
    float x = static_cast<float>(std::rand() % window.getSize().x);
    float y = static_cast<float>(std::rand() % window.getSize().y);
    this->sprite.setPosition(x, y);
    this->sprite.setScale(2.0f, 2.0f);

    // Label
    this->label.setFont(font);
    this->label.setString("Cliente " + std::to_string(index_customer + 1));
    this->label.setCharacterSize(20);
    this->label.setFillColor(sf::Color::Black);
    this->label.setPosition(x, y - 30);

}

Customer::~Customer() {}