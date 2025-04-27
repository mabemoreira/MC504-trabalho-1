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
#include "../include/sfml_utils.hpp"
#include "../include/Customer.hpp"
#include "../include/Chef.hpp"


namespace fs = std::filesystem;
using namespace std;

int main()
{
    std::map<std::string, int> signals;

    signals["init"] = 0;
    signals["end"] = 0;
    signals["n_porcoes"] = 0;
    signals["n_alunos"] = 0;
    signals["n_chefs"] = 0;
    signals["porcoes_por_aluno"] = 0;

    std::vector<Customer> customers;
    std::vector<Chef> chefs;
    sf::Clock clock;

    std::thread signalThread(receiveSignals, std::ref(signals), std::ref(customers), std::ref(chefs), std::ref(clock));
    signalThread.detach();

    while (!signals["init"])
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Iniciando Bandeco Simulator..." << std::endl;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Bandeco Simulator", sf::Style::Fullscreen);

    std::srand(std::time(nullptr)); // Inicializa o gerador de números aleatórios

    sf::Font font = loadFont("../assets/fonts/Minecraft.ttf");
    std::vector<std::string> customerFiles = getCustomerFiles(); // Lista os arquivos na pasta assets/customers

    for (int i = 0; i < signals["n_alunos"]; ++i)
        customers.emplace_back(i, customerFiles, font, window);
    for (int i = 0; i < signals["n_chefs"]; ++i)
        chefs.emplace_back(i, font);

    std::vector<sf::Sprite> potSprites(signals["n_chefs"]);
    std::vector<sf::Text> potLabels(signals["n_chefs"]); // Labels para as panelas
    std::vector<sf::Sprite> foodSprites(signals["n_alunos"]); // Sprites para os alimentos

    for (int i = 0; i < signals["n_alunos"]; ++i)
    {
        int randomIndex = std::rand() % customerFiles.size();
        if (!customers[i].getTexture().loadFromFile(customerFiles[randomIndex]))
            return -1;

        customers[i].getSprite().setTexture(customers[i].getTexture());

        customers[i].getSprite().setOrigin(customers[i].getSprite().getGlobalBounds().width / 2, customers[i].getSprite().getGlobalBounds().height / 2);
        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        customers[i].getSprite().setPosition(x, y);
        customers[i].getLabel().setOrigin(customers[i].getLabel().getGlobalBounds().width / 2, customers[i].getLabel().getGlobalBounds().height / 2);
        customers[i].getLabel().setPosition(x, y - 50); // Posição acima do sprite
    }


    // Velocidade dos chefs
    float chefSpeed = 120.0f;


    sf::Texture foodTexture;
    if (!foodTexture.loadFromFile("../assets/PixelFood.png"))
        return -1;

    for (int i = 0; i < signals["n_alunos"]; ++i)
    {
        int columns = foodTexture.getSize().x / 16;
        int rows = foodTexture.getSize().y / 16;

        int col = std::rand() % columns;
        int row = std::rand() % rows;

        foodSprites[i].setTexture(foodTexture);
        foodSprites[i].setTextureRect(sf::IntRect(
            col * 16,
            row * 16,
            16,
            16));
        foodSprites[i].setScale(1.2f, 1.2f);
    }

    // Carrega a textura da panela
    sf::Texture potTexture;
    if (!potTexture.loadFromFile("../assets/panela.png"))
        return -1;

    // Carrega a textura do chef
    sf::Texture chefTexture;
    if (!chefTexture.loadFromFile("../assets/chef/chef_off.png"))
        return -1;

    // Carrega a textura do chef ativo
    sf::Texture activeChefTexture;
    if (!activeChefTexture.loadFromFile("../assets/chef/chef_on.png"))
        return -1;

    sf::Texture returningChefTexture;
    if (!returningChefTexture.loadFromFile("../assets/chef/chef_returning.png"))
        return -1;

    // Cria signals["n_chefs"] sprites de panelas
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        // Configura o sprite da panela
        potSprites[i].setTexture(potTexture);

        // Define a posição para enfileirar horizontalmente no meio da tela
        float potX = static_cast<float>(50 + i * 150);                // Espaçamento horizontal de 150px entre as panelas
        float potY = static_cast<float>(window.getSize().y / 2 - 50); // Centraliza verticalmente
        potSprites[i].setPosition(potX, potY);
        potSprites[i].setOrigin(potSprites[i].getGlobalBounds().width / 2, potSprites[i].getGlobalBounds().height / 2);
        potSprites[i].setScale(3.0f, 3.0f);

        potLabels[i].setFont(font);
        potLabels[i].setString("Panela " + std::to_string(i + 1));
        potLabels[i].setOrigin(potLabels[i].getGlobalBounds().width / 2, potLabels[i].getGlobalBounds().height / 2);
        potLabels[i].setCharacterSize(20);
        potLabels[i].setFillColor(sf::Color::Black);
        potLabels[i].setPosition(potX, potY - 50); // Posição acima do sprite
    }

    // Cria signals["n_chefs"] sprites de chefs
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        // Configura o sprite do chef
        chefs[i].getSprite().setTexture(chefTexture);

        // Define a posição para enfileirar verticalmente na direita
        float chefX = static_cast<float>(window.getSize().x - 200); // Posição fixa na direita
        float chefY = static_cast<float>(50 + i * 150);             // Espaçamento vertical de 150px entre os chefs
        chefs[i].getSprite().setPosition(chefX, chefY);

        chefs[i].getSprite().setScale(1.5f, 1.5f);

        // Configura o label do chef
        chefs[i].getLabel().setFont(font);
        chefs[i].getLabel().setString("Chef " + std::to_string(i + 1));
        chefs[i].getLabel().setCharacterSize(20);
        chefs[i].getLabel().setFillColor(sf::Color::Black);
        chefs[i].getLabel().setPosition(chefX, chefY - 30); // Posição acima do sprite
    }

    // Guarde as posições iniciais dos chefs
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        chefs[i].setRestPosition(chefs[i].getSprite().getPosition());

        // Inicie com tempo de descanso aleatório para que eles não se movam todos ao mesmo tempo
        chefs[i].setRestTimer(static_cast<float>(std::rand() % 5) + 3.0f);
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
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Verifica se o botão foi clicado
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (closeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        window.close();
                    }
                }
            }
        }
        float deltaTime = clock.restart().asSeconds(); // Tempo desde o último frame
        float customerSpeed = 200.0f;                  // Velocidade dos clientes
        // Atualização da lógica dos clientes
        for (int i = 0; i < signals["n_alunos"]; ++i)
        {
           
            switch (customers[i].getState())
            {
            case 0:
                break;
            case 1:
                {sf::Vector2f potPos = potSprites[customers[i].getTargetPot()].getPosition();
                sf::Vector2f  costumerPos = customers[i].getSprite().getPosition();
                float distance = std::sqrt(std::pow(potPos.x - costumerPos.x, 2) + std::pow(potPos.y - costumerPos.y, 2));
                float speed = distance / 2;
                moveCostumerToPot(customers[i].getSprite(), customers[i].getLabel(), potSprites[customers[i].getTargetPot()], speed, deltaTime, i);}
                break;
            case 2:
                break;
            case 3:
                moveCostumerAfterEat(customers[i].getSprite(), customers[i].getLabel(), foodSprites[i], customerSpeed, deltaTime, i);
                break;
            default:
                break;
            }            

        }

        // Atualização da lógica dos chefs
        for (int i = 0; i < signals["n_chefs"]; ++i)
        {
            switch (chefs[i].getState())
            {
            case 0:
                break;
            case 1:
                moveChefToPot(chefs[i].getSprite(), chefs[i].getLabel(), activeChefTexture, potSprites[chefs[i].getTargetPot()], chefSpeed, deltaTime, i);
                break;
            case 2:
                break;
            case 3:
                returnChefToRest(chefs[i].getSprite(), chefs[i].getLabel(), returningChefTexture, chefTexture, chefSpeed, deltaTime, i, chefs[i].getRestPosition());
                break;
        
            default:
                break;
            }

        }
        window.clear(sf::Color(246, 241, 230)); // Fundo bege

        // Desenha os sprites e labels dos clientes
        for (int i = 0; i < signals["n_alunos"]; ++i)
        {
            window.draw(customers[i].getSprite());
            window.draw(customers[i].getLabel());
        }

        // Desenha os sprites e labels dos alimentos
        for (int i = 0; i < signals["n_alunos"]; ++i)
        {
            if (customers[i].getState() == 3) // Saindo 
            {
                window.draw(foodSprites[i]);
            }
        }

        // Desenha os sprites e labels das panelas
        for (int i = 0; i < signals["n_chefs"]; ++i)
        {
            window.draw(potSprites[i]);
            window.draw(potLabels[i]);
        }

        // Desenha os sprites e labels dos chefs
        for (int i = 0; i < signals["n_chefs"]; ++i)
        {
            window.draw(chefs[i].getSprite());
            window.draw(chefs[i].getLabel());
        }

        // Desenha o botão de fechar
        window.draw(closeButton);
        window.display();
        
    }
    return 0;
}