#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>
#include <iostream>
#include <thread>

#define MAX_PEOPLE 10
#define MAX_CHEFS 10
#define MAX_POTS 5

namespace fs = std::filesystem;

void receiveSignals()
{
    sf::TcpListener listener;
    if (listener.listen(53000) != sf::Socket::Done) {
        std::cerr << "Erro ao iniciar o servidor!" << std::endl;
        return;
    }

    std::cout << "Servidor rodando na porta 53000..." << std::endl;

    sf::TcpSocket client;
    while (true) {
        if (listener.accept(client) == sf::Socket::Done) {
            char buffer[128];
            std::size_t received;
            if (client.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
                buffer[received] = '\0'; // Garante que a mensagem é uma string
                std::string message(buffer);
                std::cout << "Mensagem recebida: " << message << std::endl;
            }
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Bandeco Simulator", sf::Style::Fullscreen);

    // Inicializa o gerador de números aleatórios
    std::srand(std::time(nullptr));

    // Carrega a fonte para os labels
    sf::Font font;
    if (!font.loadFromFile("../assets/fonts/Minecraft.ttf"))
        return -1;

    std::thread signalThread(receiveSignals);
    signalThread.detach();

    // Lista os arquivos na pasta assets/customers
    std::vector<std::string> customerFiles;
    for (const auto& entry : fs::directory_iterator("../assets/customers"))
        if (entry.is_regular_file())
            customerFiles.push_back(entry.path().string());


    std::vector<sf::Texture> customerTextures(MAX_PEOPLE);
    std::vector<sf::Sprite> customerSprites(MAX_PEOPLE);
    std::vector<sf::Text> customerLabels(MAX_PEOPLE); // Labels para os clientes
    std::vector<sf::Sprite> potSprites(MAX_CHEFS);
    std::vector<sf::Text> potLabels(MAX_CHEFS); // Labels para as panelas
    std::vector<sf::Sprite> chefSprites(MAX_CHEFS);
    std::vector<sf::Text> chefLabels(MAX_CHEFS); // Labels para os chefs

    // Carrega as texturas aleatoriamente da lista de arquivos de clientes
    for (int i = 0; i < MAX_PEOPLE; ++i) {
        int randomIndex = std::rand() % customerFiles.size();
        if (!customerTextures[i].loadFromFile(customerFiles[randomIndex]))
            return -1;

        customerSprites[i].setTexture(customerTextures[i]);

        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        customerSprites[i].setPosition(x, y);

        customerSprites[i].setScale(2.0f, 2.0f);

        // Configura o label do cliente
        customerLabels[i].setFont(font);
        customerLabels[i].setString("Cliente " + std::to_string(i + 1));
        customerLabels[i].setCharacterSize(20);
        customerLabels[i].setFillColor(sf::Color::Black);
        customerLabels[i].setPosition(x, y - 30); // Posição acima do sprite
    }

    // Carrega a textura da panela
    sf::Texture potTexture;
    if (!potTexture.loadFromFile("../assets/panela.png"))
        return -1;

    // Carrega a textura do chef
    sf::Texture chefTexture;
    if (!chefTexture.loadFromFile("../assets/chef/chef_off.png"))
        return -1;

    // Cria MAX_CHEFS sprites de panelas
    for (int i = 0; i < MAX_CHEFS; ++i) {
        // Configura o sprite da panela
        potSprites[i].setTexture(potTexture);

        // Define a posição para enfileirar horizontalmente no meio da tela
        float potX = static_cast<float>(50 + i * 150); // Espaçamento horizontal de 150px entre as panelas
        float potY = static_cast<float>(window.getSize().y / 2 - 50); // Centraliza verticalmente
        potSprites[i].setPosition(potX, potY);

        potSprites[i].setScale(3.0f, 3.0f);

        potLabels[i].setFont(font);
        potLabels[i].setString("Panela " + std::to_string(i + 1));
        potLabels[i].setCharacterSize(20);
        potLabels[i].setFillColor(sf::Color::Black);
        potLabels[i].setPosition(potX, potY - 30); // Posição acima do sprite
    }

    // Cria MAX_CHEFS sprites de chefs
    for (int i = 0; i < MAX_CHEFS; ++i) {
        // Configura o sprite do chef
        chefSprites[i].setTexture(chefTexture);

        // Define a posição para enfileirar verticalmente na direita
        float chefX = static_cast<float>(window.getSize().x - 200); // Posição fixa na direita
        float chefY = static_cast<float>(50 + i * 150); // Espaçamento vertical de 150px entre os chefs
        chefSprites[i].setPosition(chefX, chefY);

        chefSprites[i].setScale(1.5f, 1.5f);

        // Configura o label do chef
        chefLabels[i].setFont(font);
        chefLabels[i].setString("Chef " + std::to_string(i + 1));
        chefLabels[i].setCharacterSize(20);
        chefLabels[i].setFillColor(sf::Color::Black);
        chefLabels[i].setPosition(chefX, chefY - 30); // Posição acima do sprite
    }

    // Carrega a textura do botão de fechar
    sf::Texture closeButtonTexture;
    if (!closeButtonTexture.loadFromFile("../assets/buttons/x.png"))
        return -1;

    // Cria o sprite do botão de fechar
    sf::Sprite closeButton(closeButtonTexture);
    closeButton.setPosition(10.0f, 10.0f); // Posição no canto superior esquerdo
    closeButton.setScale(3.0f, 3.0f);

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Verifica se o botão foi clicado
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (closeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        window.close();
                    }
                }
            }
        }

        window.clear(sf::Color(246, 241, 230)); // Fundo bege

        // Desenha os sprites e labels dos clientes
        for (int i = 0; i < MAX_PEOPLE; ++i) {
            window.draw(customerSprites[i]);
            window.draw(customerLabels[i]);
        }

        // Desenha os sprites e labels das panelas
        for (int i = 0; i < MAX_CHEFS; ++i) {
            window.draw(potSprites[i]);
            window.draw(potLabels[i]);
        }

        // Desenha os sprites e labels dos chefs
        for (int i = 0; i < MAX_CHEFS; ++i) {
            window.draw(chefSprites[i]);
            window.draw(chefLabels[i]);
        }

        // Desenha o botão de fechar
        window.draw(closeButton);
        window.display();
    }
    return 0;
}