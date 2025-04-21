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

bool moveTowards(sf::Sprite& sprite, sf::Vector2f targetPos, float speed, float deltaTime) {
    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f direction = targetPos - currentPos;

    // Calcula a distância até o alvo
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Verifica se já está no alvo ou muito perto
    // Se a distância restante for menor que a distância que moveria neste frame,
    // apenas coloque o sprite no alvo para evitar ultrapassar.
    if (distance <= speed * deltaTime || distance == 0.f) {
        sprite.setPosition(targetPos);
        return true; // Alvo alcançado
    } else {
        // Normaliza o vetor de direção (torna o comprimento 1)
        sf::Vector2f normalizedDirection = direction / distance;
        // Calcula o vetor de movimento para este frame
        sf::Vector2f movement = normalizedDirection * speed * deltaTime;
        // Move o sprite
        sprite.move(movement);
        return false; // Alvo ainda não alcançado
    }
}


bool moveCostumerToPot(sf::Sprite& costumerSprite, sf::Text& costumerLabel, sf::Sprite& potSprite, float costumerVelocity, float deltaTime, int costumerIndex) {
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

    if(!result) {
        // Se o cliente chegou à panela, atualiza a posição do label
        costumerLabel.setString("Cliente " + std::to_string(costumerIndex + 1) + " (Indo a panela)");
    } else {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = costumerLabel.getString();
        costumerLabel.setString("Cliente " + std::to_string(costumerIndex + 1) + " (Enchendo a bandeja)");
    }
    return result;

}

bool watingToEat(sf::Sprite& costumerSprite, sf::Sprite& potSprite, float costumerVelocity, float deltaTime) {
    // Se a panela está vazia, espera encher
    // Se a panela está cheia, ficar enchendo a bandeja
    // Se já passou o tempo de espera, vai embora
    return false;
}



bool moveCostumerAfterEat(sf::Sprite& costumerSprite, sf::Text& costumerLabel, sf::Sprite& foodSprite ,float velocity, float deltaTime, int costumerIndex) {
    costumerLabel.setString("Cliente " + std::to_string(costumerIndex) + " (indo a mesa)");
    sf::Vector2f targetPos;
    targetPos.x = -costumerSprite.getGlobalBounds().width - 60;
    targetPos.y = costumerSprite.getPosition().y;

    bool moveCostumer =  moveTowards(costumerSprite, targetPos, velocity, deltaTime);
    
    foodSprite.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y);
    costumerLabel.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y - 30);
    
    return moveCostumer;
}

bool moveChefToPot(sf::Sprite& chefSprite, sf::Text& chefLabel,sf::Texture& activeChefTexture, sf::Sprite& potSprite, float chefVelocity, float deltaTime, int chefIndex) {
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

    if(!result) {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = chefLabel.getString();
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Indo a panela)");
    } else {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = chefLabel.getString();
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Enchendo a bandeja)");
    }
    return result;

}

bool returnChefToRest(sf::Sprite& chefSprite, sf::Text& chefLabel, sf::Texture& returningChef,sf::Texture& sleepingChef, float chefVelocity, float deltaTime, int chefIndex, sf::Vector2f inicialPos) {
    chefSprite.setTexture(returningChef);
    chefSprite.setScale(1.5f, 1.5f);

    bool result = moveTowards(chefSprite, inicialPos, chefVelocity, deltaTime);
    
    if (result){
        chefSprite.setTexture(sleepingChef);
        chefSprite.setScale(1.5f, 1.5f);
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1));
    } else {
        // Se o cliente chegou à panela, atualiza a posição do label
        std::string currentLabelText = chefLabel.getString();
        chefLabel.setString("Chef " + std::to_string(chefIndex + 1) + " (Indo descansar)");
    }
    return result;
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
    std::vector<sf::Sprite> foodSprites(signals["n_alunos"]); // Sprites para os alimentos
    // Estruturas para controlar o estado de cada cliente
    std::vector<int> customerTargetPot(signals["n_alunos"], -1); // -1 significa sem panela alvo
    std::vector<bool> customerMoving(signals["n_alunos"], false);
    // Adicione estas estruturas logo após a criação dos outros vetores de controle
    std::vector<bool> customerEating(signals["n_alunos"], false); // Cliente está comendo
    std::vector<float> customerEatingTimer(signals["n_alunos"], 0.0f); // Tempo restante comendo
    std::vector<bool> customerLeaving(signals["n_alunos"], false); // Cliente está saindo
    // Estruturas para controlar o estado de cada chef
    std::vector<int> chefState(signals["n_chefs"], 0);     // 0: descansando, 1: indo para panela, 2: cozinhando, 3: retornando
    std::vector<int> chefTargetPot(signals["n_chefs"], -1); // -1 significa sem panela alvo
    std::vector<float> chefCookingTimer(signals["n_chefs"], 0.0f); // Tempo restante cozinhando
    std::vector<float> chefRestTimer(signals["n_chefs"], 0.0f); // Tempo restante descansando
    std::vector<sf::Vector2f> chefRestPositions(signals["n_chefs"]); // Posições iniciais/de descanso

    // Velocidade dos chefs
    float chefSpeed = 120.0f;

    sf::Clock clock;
    // Carrega as texturas aleatoriamente da lista de arquivos de clientes
    for (int i = 0; i < signals["n_alunos"]; ++i)
    {
        int randomIndex = std::rand() % customerFiles.size();
        if (!customerTextures[i].loadFromFile(customerFiles[randomIndex]))
            return -1;

        customerSprites[i].setTexture(customerTextures[i]);

        customerSprites[i].setOrigin(customerSprites[i].getGlobalBounds().width / 2, customerSprites[i].getGlobalBounds().height / 2);
        float x = static_cast<float>(std::rand() % window.getSize().x);
        float y = static_cast<float>(std::rand() % window.getSize().y);
        customerSprites[i].setPosition(x, y);

        customerSprites[i].setScale(2.0f, 2.0f);
        // // Inicializa velocidade (adicione estas linhas)
        // float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
        // customerVelocities[i] = sf::Vector2f(std::cos(angle) * customerSpeed, std::sin(angle) * customerSpeed);
    
        // Configura o label do cliente
        customerLabels[i].setFont(font);
        customerLabels[i].setString("Cliente " + std::to_string(i + 1));
        customerLabels[i].setCharacterSize(20);
        customerLabels[i].setFillColor(sf::Color::Black);
        customerLabels[i].setPosition(x, y - 30); // Posição acima do sprite
    }



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
        16
    ));
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

    
    // Guarde as posições iniciais dos chefs
    for (int i = 0; i < signals["n_chefs"]; ++i) {
        chefRestPositions[i] = chefSprites[i].getPosition();

    // Inicie com tempo de descanso aleatório para que eles não se movam todos ao mesmo tempo
    chefRestTimer[i] = static_cast<float>(std::rand() % 5) + 3.0f;
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
        {
    
    float deltaTime = clock.restart().asSeconds(); // Tempo desde o último frame
    float customerSpeed = 100.0f; // Velocidade dos clientes     
    // Atualização da lógica dos clientes
    for (int i = 0; i < signals["n_alunos"]; ++i)
    {
        // Se o cliente está comendo
        if (customerEating[i])
        {
            customerEatingTimer[i] -= deltaTime;
            
            // Se terminou de comer
            if (customerEatingTimer[i] <= 0.0f)
            {
                // Mudança de estado: comendo → saindo
                customerEating[i] = false;
                customerLeaving[i] = true;
                std::cout << "Cliente " << (i+1) << " terminou de comer e está saindo" << std::endl;
                customerLabels[i].setString("Cliente " + std::to_string(i + 1) + " (saindo)");
            }
            continue; // Vai para o próximo cliente
        }
        
        // Se o cliente está saindo
        if (customerLeaving[i])
        {
            // Usa a função moveCostumerAfterEat para mover o cliente para fora da tela
            bool leftScreen = moveCostumerAfterEat(customerSprites[i],  customerLabels[i], foodSprites[i], customerSpeed, deltaTime, i);
            
            // Se o cliente saiu da tela
            if (leftScreen)
            {
                // Não reposiciona o cliente - deixa ele fora da tela
                // Apenas reseta o estado para não estar mais "saindo"
                customerLeaving[i] = false;
                customerLabels[i].setString("Cliente " + std::to_string(i + 1));
                
                // Não precisa atribuir nova velocidade aqui, já que o cliente está fora da tela
            }
            continue; // Vai para o próximo cliente
            if (!customerMoving[i] && (std::rand() % 100 < 2)) // 2% de chance a cada frame
            {
                // Verifica se o cliente está fora da tela (posição x negativa)
                if (customerSprites[i].getPosition().x < 0)
                {
                    // Reposiciona o cliente no lado direito da tela
                    float newX = window.getSize().x + 10;
                    float newY = static_cast<float>(std::rand() % (int)(window.getSize().y - 100)) + 50;
                    customerSprites[i].setPosition(newX, newY);
                    
                    // Atualiza o label para a nova posição
                    customerLabels[i].setPosition(newX, newY - 30);
                }
                
                // Escolhe uma panela aleatória
                customerTargetPot[i] = std::rand() % signals["n_chefs"];
                customerMoving[i] = true;
                std::cout << "Cliente " << (i+1) << " decidiu ir para a panela " << (customerTargetPot[i]+1) << std::endl;
            }
        }
        
        // Se o cliente não está se movendo para uma panela nem comendo nem saindo
        if (!customerMoving[i] && (std::rand() % 100 < 2)) // 2% de chance a cada frame
        {
            // Escolhe uma panela aleatória
            customerTargetPot[i] = std::rand() % signals["n_chefs"];
            customerMoving[i] = true;
            std::cout << "Cliente " << (i+1) << " decidiu ir para a panela " << (customerTargetPot[i]+1) << std::endl;
        }
        
        // Se o cliente está se movendo para uma panela
        if (customerMoving[i])
        {
            // Usando sua função moveCostumerToPot
            bool reached = moveCostumerToPot(customerSprites[i], customerLabels[i],potSprites[customerTargetPot[i]], 
                                          customerSpeed, deltaTime, i);

            
            // Se o cliente chegou à panela
            if (reached)
            {
                std::cout << "Cliente " << (i+1) << " chegou à panela " << (customerTargetPot[i]+1) << std::endl;
                
                // Muda para o estado "comendo"
                customerMoving[i] = false;
                customerEating[i] = true;
                customerEatingTimer[i] = 2.0f; // 2 segundos comendo
                
                // Atualiza o label
                customerLabels[i].setString("Cliente " + std::to_string(i + 1) + " (comendo)");
            }
        }
    }

    // Atualização da lógica dos chefs
    for (int i = 0; i < signals["n_chefs"]; ++i)
    {
        // Estado 0: Chef está descansando
        if (chefState[i] == 0) {
            chefRestTimer[i] -= deltaTime;
            
            // Se terminou o descanso
            if (chefRestTimer[i] <= 0.0f) {
                // Escolhe uma panela aleatória (ou pode ser a própria panela do chef)
                chefTargetPot[i] = i; // Cada chef cuida de sua própria panela
                chefState[i] = 1; // Muda para estado "indo para panela"
                std::cout << "Chef " << (i+1) << " está indo para a panela " << (chefTargetPot[i]+1) << std::endl;
            }
        }
        
        // Estado 1: Chef está indo para a panela
        else if (chefState[i] == 1) {
            bool reached = moveChefToPot(chefSprites[i], chefLabels[i], activeChefTexture, potSprites[chefTargetPot[i]], 
                                        chefSpeed, deltaTime, i);
            
            // Se chegou à panela
            if (reached) {
                chefState[i] = 2; // Muda para estado "cozinhando"
                chefCookingTimer[i] = 3.0f + static_cast<float>(std::rand() % 2); // 3-5 segundos cozinhando
                std::cout << "Chef " << (i+1) << " está cozinhando na panela " << (chefTargetPot[i]+1) << std::endl;
            }
        }
        
        // Estado 2: Chef está cozinhando
        else if (chefState[i] == 2) {
            chefCookingTimer[i] -= deltaTime;
            
            // Se terminou de cozinhar
            if (chefCookingTimer[i] <= 0.0f) {
                chefState[i] = 3; // Muda para estado "retornando"
                std::cout << "Chef " << (i+1) << " está retornando ao descanso" << std::endl;
            }
        }
        
        // Estado 3: Chef está retornando ao descanso
        else if (chefState[i] == 3) {
            bool returned = returnChefToRest(chefSprites[i], chefLabels[i], returningChefTexture, chefTexture, chefSpeed, 
                                            deltaTime, i, chefRestPositions[i]);
            
            // Se chegou ao local de descanso
            if (returned) {
                chefState[i] = 0; // Muda para estado "descansando"
                chefRestTimer[i] = 5.0f + static_cast<float>(std::rand() % 5); // 5-10 segundos descansando
                std::cout << "Chef " << (i+1) << " está descansando" << std::endl;
            }
        }
        
        // Atualiza a posição do label do chef
        chefLabels[i].setPosition(chefSprites[i].getPosition().x, chefSprites[i].getPosition().y - 30);
    }
                window.clear(sf::Color(246, 241, 230)); // Fundo bege
            
            // Desenha os sprites e labels dos clientes
            for (int i = 0; i < signals["n_alunos"]; ++i)
            {
                window.draw(customerSprites[i]);
                window.draw(customerLabels[i]);
            }

            // Desenha os sprites e labels dos alimentos
            for (int i = 0; i < signals["n_alunos"]; ++i) {
                if (customerLeaving[i]) {
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
                window.draw(chefSprites[i]);
                window.draw(chefLabels[i]);
            }

            // Desenha o botão de fechar
            window.draw(closeButton);
            window.display();
        }
    }
  
}
return 0;
}