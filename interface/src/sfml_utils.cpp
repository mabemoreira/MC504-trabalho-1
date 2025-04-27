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
namespace fs = std::filesystem;
using namespace std;

bool moveTowards(sf::Sprite &sprite, sf::Vector2f targetPos, float speed, float deltaTime)
{
    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f direction = targetPos - currentPos;

    // Calcula a distância até o alvo
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Verifica se já está no alvo ou muito perto
    // Se a distância restante for menor que a distância que moveria neste frame,
    // apenas coloque o sprite no alvo para evitar ultrapassar.
    if (distance <= speed * deltaTime || distance == 0.f)
    {
        sprite.setPosition(targetPos);
        return true; // Alvo alcançado
    }
    else
    {
        // Normaliza o vetor de direção (torna o comprimento 1)
        sf::Vector2f normalizedDirection = direction / distance;
        // Calcula o vetor de movimento para este frame
        sf::Vector2f movement = normalizedDirection * speed * deltaTime;
        // Move o sprite
        sprite.move(movement);
        return false; // Alvo ainda não alcançado
    }
}

bool moveCostumerToPot(sf::Sprite &costumerSprite, sf::Text &costumerLabel, sf::Sprite &potSprite, float costumerVelocity, float deltaTime, int costumerIndex)
{
    sf::Vector2f potPos = potSprite.getPosition();
    sf::FloatRect potBounds = potSprite.getGlobalBounds();
    sf::FloatRect customerBounds = costumerSprite.getGlobalBounds();

    // Escolhendo se o boneco para mais a esquerda ou direita da panela
    potPos.x += (std::rand() % 2 == 0) ? (customerBounds.width + 20) : (-customerBounds.width - 20);
    // Escolhendo se a pessoa come pela parte de cima ou de baixo da panela
    if (customerBounds.top <= potPos.y)
        potPos.y -= potBounds.height / 2 + customerBounds.height / 2;
    else
        potPos.y += potBounds.height / 2 + customerBounds.height / 2;

    bool result = moveTowards(costumerSprite, potPos, costumerVelocity, deltaTime);

    costumerLabel.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y - 30);

    if (!result)
    {
        // Se o cliente chegou à panela, atualiza a posição do label
        costumerLabel.setString("Cliente " + std::to_string(costumerIndex + 1) + " (Indo a panela)");
    }
    else
    {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = costumerLabel.getString();
        costumerLabel.setString("Cliente " + std::to_string(costumerIndex + 1) + " (Enchendo a bandeja)");
    }
    return result;
}

bool watingToEat(sf::Sprite &costumerSprite, sf::Sprite &potSprite, float costumerVelocity, float deltaTime)
{
    // Se a panela está vazia, espera encher
    // Se a panela está cheia, ficar enchendo a bandeja
    // Se já passou o tempo de espera, vai embora
    return false;
}

bool moveCostumerAfterEat(sf::Sprite &costumerSprite, sf::Text &costumerLabel, sf::Sprite &foodSprite, float velocity, float deltaTime, int costumerIndex)
{
    costumerLabel.setString("Cliente " + std::to_string(costumerIndex) + " (indo a mesa)");
    sf::Vector2f targetPos;
    targetPos.x = -costumerSprite.getGlobalBounds().width - 60;
    targetPos.y = costumerSprite.getPosition().y;

    bool moveCostumer = moveTowards(costumerSprite, targetPos, velocity, deltaTime);

    foodSprite.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y);
    costumerLabel.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y - 30);

    if (moveCostumer)
        costumerLabel.setString("Cliente " + std::to_string(costumerIndex));

    return moveCostumer;
}

bool moveChefToPot(sf::Sprite &chefSprite, sf::Text &chefLabel, sf::Texture &activeChefTexture, sf::Sprite &potSprite, float chefVelocity, float deltaTime, int chefIndex)
{
    chefSprite.setTexture(activeChefTexture);
    chefSprite.setScale(1.5f, 1.5f);

    sf::Vector2f potPos = potSprite.getPosition();
    sf::FloatRect potBounds = potSprite.getGlobalBounds();
    sf::FloatRect chefBounds = chefSprite.getGlobalBounds();

    // Escolhendo se a pessoa come pela parte de cima ou de baixo da panela
    if (chefBounds.top <= potPos.y)
        potPos.y -= potBounds.height / 2 + chefBounds.height / 2 + 10;

    bool result = moveTowards(chefSprite, potPos, chefVelocity, deltaTime);

    chefLabel.setPosition(chefSprite.getPosition().x, chefSprite.getPosition().y - 30);

    if (!result)
    {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = chefLabel.getString();
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Indo a panela)");
    }
    else
    {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = chefLabel.getString();
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Enchendo a bandeja)");
    }
    return result;
}

bool returnChefToRest(sf::Sprite &chefSprite, sf::Text &chefLabel, sf::Texture &returningChef, sf::Texture &sleepingChef, float chefVelocity, float deltaTime, int chefIndex, sf::Vector2f inicialPos)
{
    chefSprite.setTexture(returningChef);
    chefSprite.setScale(1.5f, 1.5f);

    bool result = moveTowards(chefSprite, inicialPos, chefVelocity, deltaTime);

    if (result)
    {
        chefSprite.setTexture(sleepingChef);
        chefSprite.setScale(1.5f, 1.5f);
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1));
    }
    else
    {
        // Se o cliente chegou à panela, atualiza a posição do label
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Indo descansar)");
    }
    return result;
}

void receiveSignals(std::map<std::string, int> &signals, std::vector<Customer> &customers, std::vector<Chef> &chefs, sf::Clock &clock)
{
    sf::TcpListener listener;
    if (listener.listen(53002) != sf::Socket::Done)
    {
        std::cerr << "Erro ao iniciar o servidor!" << std::endl;
        return;
    }

    std::cout << "Servidor rodando na porta 53002..." << std::endl;

    sf::TcpSocket client;
    while (true)
    {
        float deltaTime = clock.restart().asSeconds();


        if (listener.accept(client) == sf::Socket::Done)
        {
            char buffer[128];
            std::size_t received;
            if (client.receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
            {
                buffer[received] = '\0';
                std::string message(buffer);
                std::cout << "Mensagem recebida: " << message << std::endl;

                std::vector<std::string> parsedMessage;
                std::istringstream iss(message);
                std::string palavra;
                while (iss >> palavra)
                    parsedMessage.push_back(palavra);
                std::string command = parsedMessage[0];

                if (command == "init")
                {
                    signals["init"] = 1;
                    signals["n_porcoes"] = std::stoi(parsedMessage[1]);
                    signals["n_alunos"] = std::stoi(parsedMessage[2]);
                    signals["n_chefs"] = std::stoi(parsedMessage[3]);
                    signals["porcoes_por_aluno"] = std::stoi(parsedMessage[4]);

                    std::cout << "Iniciando com " << signals["n_porcoes"] << " porções, "
                              << signals["n_alunos"] << " alunos, "
                              << signals["n_chefs"] << " chefs e "
                              << signals["porcoes_por_aluno"] << " porções por aluno" << std::endl;
                }
                if (command == "end")
                {
                    signals["end"] = 1;
                    // fazer algum comando caso a gente queria alguma tela após o fim do jogo
                }
                if (command == "putServingsInPot")
                {
                    // implemetar função para colocar porções na panela
                }
                if (command == "returnChefToRest")
                {
                    int chefId = std::stoi(parsedMessage[1]);
                    chefs[chefId].setState(3);
                    chefs[chefId].setTargetPot(-1);
                }
                if (command == "returnCustomerToRest")
                {
                    int customerId = std::stoi(parsedMessage[1]);
                    customers[customerId].setState(3);
                    customers[customerId].setTargetPot(-1);

                }
                if (command == "noFood")
                {
                    int chefId = std::stoi(parsedMessage[1]);
                    chefs[chefId].setState(1);
                    chefs[chefId].setTargetPot(chefId);
                }
                if (command == "getFood")
                {
                    int customerId = std::stoi(parsedMessage[1]);
                    int potId = std::stoi(parsedMessage[2]);

                    customers[customerId].setTargetPot(potId);
                    customers[customerId].setState(1); // 1: indo para panela
                    

                }
            }
        }
    }
}

sf::Font loadFont(const std::string& fontPath)
{
    sf::Font font;
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Erro ao carregar a fonte!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return font;
}

std::vector<std::string> getCustomerFiles()
{
    std::vector<std::string> customerFiles;
    for (const auto &entry : fs::directory_iterator("../assets/customers"))
        if (entry.is_regular_file())
            customerFiles.push_back(entry.path().string());
    return customerFiles;
}

