#include <SFML/Graphics.hpp>
#include <iostream>
#include "../include/Customer.hpp"

namespace fs = std::filesystem;
using namespace std;

Customer::Customer(int index_customer, std::vector<std::string>& customerFiles, const sf::Font& font, sf::RenderWindow& window)
    : moving(false), eating(false), leaving(false), targetPot(-1), eatingTimer(0.0f)
{
    // Carrega as texturas aleatoriamente da lista de arquivos de clientes
    int randomIndex = std::rand() % customerFiles.size();
    this->spriteFile = customerFiles[randomIndex];
    std::cout << "Carregando cliente: " << this->spriteFile << std::endl;

    if (!this->texture.loadFromFile(this->spriteFile)) {
        std::cerr << "Erro ao carregar a textura do cliente: " << this->spriteFile << std::endl;
        // Define uma textura padrão ou deixa o sprite vazio
        this->texture.create(500, 500); // Cria uma textura branca de 50x50
        std::runtime_error("Erro ao carregar a textura do cliente");
    }
    this->sprite.setTexture(this->texture);
    this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 2, this->sprite.getGlobalBounds().height / 2);

    float x = static_cast<float>(std::rand() % window.getSize().x);
    float y = static_cast<float>(std::rand() % window.getSize().y);

    this->sprite.setPosition(x, y);
    this->sprite.setScale(2.0f, 2.0f);

    // // Inicializa velocidade (adicione estas linhas)
    // float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
    // customerVelocities[i] = sf::Vector2f(std::cos(angle) * customerSpeed, std::sin(angle) * customerSpeed);

    // Configura o label do cliente
    this->label.setFont(font);
    this->label.setString("Cliente " + std::to_string(index_customer + 1));
    this->label.setCharacterSize(20);
    this->label.setFillColor(sf::Color::Black);
    this->label.setPosition(x, y - 30); // Posição acima do sprite
}

Customer::~Customer() {}