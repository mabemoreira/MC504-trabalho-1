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

void receiveSignals(std::map<std::string, int> &signals)
{
    sf::TcpListener listener;
    if (listener.listen(53002) != sf::Socket::Done)
    {
        std::cerr << "Erro ao iniciar o servidor!" << std::endl;
        return;
    }

    std::cout << "Servidor rodando na porta 53000..." << std::endl;

    sf::TcpSocket client;
    while (true)
    {
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
                              << signals["n_alunos"] << " alunos e "
                              << signals["n_chefs"] << " chefs." << std::endl;
                }
            }
        }
    }
}

int main()
{
    std::map<std::string, int> signals;

    signals["init"] = 0;
    signals["end"] = 0;
    signals["n_porcoes"] = 0;
    signals["n_alunos"] = 0;
    signals["n_chefs"] = 0;
    signals["porcoes_por_aluno"] = 0;

    std::thread signalThread(receiveSignals, std::ref(signals));
    signalThread.detach();

    while (!signals["init"])
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Iniciando Bandeco Simulator..." << std::endl;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Bandeco Simulator", sf::Style::Fullscreen);

    // Inicializa o gerador de números aleatórios
    std::srand(std::time(nullptr));

    // Carrega a fonte para os labels
    sf::Font font;
    if (!font.loadFromFile("../assets/fonts/Minecraft.ttf"))
        return -1;

    // Lista os arquivos na pasta assets/customers
    std::vector<std::string> customerFiles;
    for (const auto &entry : fs::directory_iterator("../assets/customers"))
        if (entry.is_regular_file())
            customerFiles.push_back(entry.path().string());

    std::vector<sf::Texture> customerTextures(signals["n_alunos"]);
    std::vector<sf::Sprite> customerSprites(signals["n_alunos"]);
    std::vector<sf::Text> customerLabels(signals["n_alunos"]);         // Labels para os clientes
    std::vector<sf::Vector2f> customerVelocities(signals["n_alunos"]); // Add this line
    std::vector<sf::Sprite> potSprites(signals["n_chefs"]);
    std::vector<sf::Text> potLabels(signals["n_chefs"]); // Labels para as panelas
    std::vector<sf::Sprite> chefSprites(signals["n_chefs"]);
    std::vector<sf::Text> chefLabels(signals["n_chefs"]); // Labels para os chefs

    // Carrega as texturas aleatoriamente da lista de arquivos de clientes
    float customerSpeed = 20.0f; // Velocidade dos clientes
    for (int i = 0; i < signals["n_alunos"]; ++i)
    {
        int randomIndex = std::rand() % customerFiles.size();
        if (!customerTextures[i].loadFromFile(customerFiles[randomIndex]))
            return -1;

        customerSprites[i].setTexture(customerTextures[i]);

        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        customerSprites[i].setPosition(x, y);

        customerSprites[i].setScale(2.0f, 2.0f);

        // Initialize velocity (Add these lines)
        float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f; // Random angle
        customerVelocities[i] = sf::Vector2f(std::cos(angle) * customerSpeed, std::sin(angle) * customerSpeed);

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

    // Cria signals["n_chefs"] sprites de panelas
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        // Configura o sprite da panela
        potSprites[i].setTexture(potTexture);

        // Define a posição para enfileirar horizontalmente no meio da tela
        float potX = static_cast<float>(50 + i * 150);                // Espaçamento horizontal de 150px entre as panelas
        float potY = static_cast<float>(window.getSize().y / 2 - 50); // Centraliza verticalmente
        potSprites[i].setPosition(potX, potY);

        potSprites[i].setScale(3.0f, 3.0f);

        potLabels[i].setFont(font);
        potLabels[i].setString("Panela " + std::to_string(i + 1));
        potLabels[i].setCharacterSize(20);
        potLabels[i].setFillColor(sf::Color::Black);
        potLabels[i].setPosition(potX, potY - 30); // Posição acima do sprite
    }

    // Cria signals["n_chefs"] sprites de chefs
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        // Configura o sprite do chef
        chefSprites[i].setTexture(chefTexture);

        // Define a posição para enfileirar verticalmente na direita
        float chefX = static_cast<float>(window.getSize().x - 200); // Posição fixa na direita
        float chefY = static_cast<float>(50 + i * 150);             // Espaçamento vertical de 150px entre os chefs
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

    sf::Clock clock; // Create an SFML clock to track time

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
            float deltaTime = clock.restart().asSeconds(); // Get time delta

            // --- Update customer positions (Add this section) ---
            for (int i = 0; i < signals["n_alunos"]; ++i)
            {
                customerSprites[i].move(customerVelocities[i] * deltaTime);

                // Boundary check
                sf::FloatRect bounds = customerSprites[i].getGlobalBounds();
                sf::Vector2f pos = customerSprites[i].getPosition();

                // Check horizontal bounds
                if (pos.x < 0)
                {
                    customerVelocities[i].x = std::abs(customerVelocities[i].x); // Move right
                    customerSprites[i].setPosition(0, pos.y);                    // Correct position
                }
                else if (pos.x + bounds.width > window.getSize().x)
                {
                    customerVelocities[i].x = -std::abs(customerVelocities[i].x);             // Move left
                    customerSprites[i].setPosition(window.getSize().x - bounds.width, pos.y); // Correct position
                }

                // Check vertical bounds
                if (pos.y < 0)
                {
                    customerVelocities[i].y = std::abs(customerVelocities[i].y); // Move down
                    customerSprites[i].setPosition(pos.x, 0);                    // Correct position
                }
                else if (pos.y + bounds.height > window.getSize().y)
                {
                    customerVelocities[i].y = -std::abs(customerVelocities[i].y);              // Move up
                    customerSprites[i].setPosition(pos.x, window.getSize().y - bounds.height); // Correct position
                }

                // Update label position to follow the sprite
                customerLabels[i].setPosition(customerSprites[i].getPosition().x, customerSprites[i].getPosition().y - 30);
            }

            window.clear(sf::Color(246, 241, 230)); // Fundo bege

            // Desenha os sprites e labels dos clientes
            for (int i = 0; i < signals["n_alunos"]; ++i)
            {
                window.draw(customerSprites[i]);
                window.draw(customerLabels[i]);
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
                window.draw(chefSprites[i]);
                window.draw(chefLabels[i]);
            }

            // Desenha o botão de fechar
            window.draw(closeButton);
            window.display();
        }
    }
    return 0;
}