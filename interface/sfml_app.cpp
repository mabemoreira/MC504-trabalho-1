#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <filesystem>

#define MAX_PEOPLE 10
#define MAX_CHEFS 10

namespace fs = std::filesystem;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Bandeco Simulator", sf::Style::Fullscreen);

    // Inicializa o gerador de números aleatórios
    std::srand(std::time(nullptr));

    // Lista os arquivos na pasta assets/customers
    std::vector<std::string> customerFiles;
    for (const auto& entry : fs::directory_iterator("../assets/customers")) {
        if (entry.is_regular_file()) {
            customerFiles.push_back(entry.path().string());
        }
    }

    // Verifica se há arquivos disponíveis
    if (customerFiles.empty()) {
        return -1; // Retorna erro se não houver arquivos na pasta
    }

    // Vetor para armazenar as texturas e sprites dos clientes
    std::vector<sf::Texture> customerTextures(MAX_PEOPLE);
    std::vector<sf::Sprite> customerSprites(MAX_PEOPLE);

    // Carrega as texturas aleatoriamente da lista de arquivos de clientes
    for (int i = 0; i < MAX_PEOPLE; ++i) {
        int randomIndex = std::rand() % customerFiles.size(); // Seleciona um arquivo aleatório
        if (!customerTextures[i].loadFromFile(customerFiles[randomIndex])) {
            return -1; // Retorna erro se a textura não for carregada
        }

        // Cria o sprite correspondente
        customerSprites[i].setTexture(customerTextures[i]);

        // Define uma posição aleatória para o sprite
        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        customerSprites[i].setPosition(x, y);

        // Aumenta o tamanho do sprite (2x maior)
        customerSprites[i].setScale(2.0f, 2.0f);
    }

    // Carrega a textura da panela
    sf::Texture potTexture;
    if (!potTexture.loadFromFile("../assets/panela.png")) {
        return -1; // Retorna erro se a textura não for carregada
    }

    // Vetor para armazenar os sprites das panelas
    std::vector<sf::Sprite> potSprites(MAX_CHEFS);

    // Cria MAX_CHEFS sprites de panelas
    for (int i = 0; i < MAX_CHEFS; ++i) {
        potSprites[i].setTexture(potTexture);

        // Define uma posição aleatória para o sprite
        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        potSprites[i].setPosition(x, y);

        // Aumenta o tamanho do sprite (1.5x maior)
        potSprites[i].setScale(1.5f, 1.5f);
    }

    // Cria um botão no canto superior direito
    sf::RectangleShape closeButton(sf::Vector2f(50.0f, 50.0f)); // Botão de 50x50 pixels
    closeButton.setFillColor(sf::Color::Red);
    closeButton.setPosition(window.getSize().x - 60.0f, 10.0f); // Posição no canto superior direito

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
                        window.close(); // Fecha a janela
                    }
                }
            }
        }

        window.clear(sf::Color(246, 241, 230)); // Fundo bege

        // Desenha os sprites dos clientes
        for (const auto& sprite : customerSprites) {
            window.draw(sprite);
        }

        // Desenha os sprites das panelas
        for (const auto& sprite : potSprites) {
            window.draw(sprite);
        }

        // Desenha o botão de fechar
        window.draw(closeButton);
        window.display();
    }
    return 0;
}