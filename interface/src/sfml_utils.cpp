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
#include <queue>
#include <mutex>
#include <condition_variable>
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

    costumerLabel.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y - 50);

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
    costumerLabel.setPosition(costumerSprite.getPosition().x, costumerSprite.getPosition().y - 50);

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
    chefLabel.setPosition(chefSprite.getPosition().x, chefSprite.getPosition().y - 30);

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
// Classes e estrutura para receber mensagens entre threads
struct Message {
    std::string command;
    std::vector<std::string> params;
};


class MessageQueue {
private:
    std::queue<Message> queue;
    std::mutex mutex;
    std::condition_variable cv;

public:
    void push(const Message& message) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(message);
        cv.notify_one();
    }

    bool pop(Message& message) {
        std::unique_lock<std::mutex> lock(mutex);
        // Espera até que haja uma mensagem ou até que o tempo limite seja atingido
        if (cv.wait_for(lock, std::chrono::milliseconds(10), [this] { return !queue.empty(); })) {
            message = queue.front();
            queue.pop();
            return true;
        }
        return false;
    }
};

// Thread para receber mensagens
void receiverThread(MessageQueue& messageQueue) {
    sf::TcpListener listener;
    if (listener.listen(53002) != sf::Socket::Done) {
        std::cerr << "Erro ao iniciar o servidor!" << std::endl;
        return;
    }

    std::cout << "Servidor rodando na porta 53002..." << std::endl;
    
    // Configurar para não bloquear
    listener.setBlocking(false);

    sf::TcpSocket client;
    while (true) {
        // Tenta aceitar conexão sem bloquear
        sf::Socket::Status status = listener.accept(client);
        
        if (status == sf::Socket::Done) {
            char buffer[128];
            std::size_t received;
            
            if (client.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
                buffer[received] = '\0';
                std::string messageStr(buffer);
                std::cout << "Mensagem recebida: " << messageStr << std::endl;

                // Parse da mensagem
                std::vector<std::string> parsedMessage;
                std::istringstream iss(messageStr);
                std::string palavra;
                while (iss >> palavra)
                    parsedMessage.push_back(palavra);
                
                // Se houver comando válido, adiciona à fila
                if (!parsedMessage.empty()) {
                    Message msg;
                    msg.command = parsedMessage[0];
                    for (size_t i = 1; i < parsedMessage.size(); i++) {
                        msg.params.push_back(parsedMessage[i]);
                    }
                    messageQueue.push(msg);
                }
            }
        }
        
        // Pequena pausa para não sobrecarregar a CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void processorThread(MessageQueue& messageQueue, std::map<std::string, int>& signals, 
                    std::vector<Customer>& customers, std::vector<Chef>& chefs) {
    while (true) {
        Message message;
        if (messageQueue.pop(message)) {
            // Processa a mensagem
            if (message.command == "init" && message.params.size() >= 4) {
                signals["init"] = 1;
                signals["n_porcoes"] = std::stoi(message.params[0]);
                signals["n_alunos"] = std::stoi(message.params[1]);
                signals["n_chefs"] = std::stoi(message.params[2]);
                signals["porcoes_por_aluno"] = std::stoi(message.params[3]);
                
                std::cout << "Iniciando com " << signals["n_porcoes"] << " porções, "
                        << signals["n_alunos"] << " alunos, "
                        << signals["n_chefs"] << " chefs e "
                        << signals["porcoes_por_aluno"] << " porções por aluno" << std::endl;
            }
            else if (message.command == "end") {
                signals["end"] = 1;
            }
            else if (message.command == "putServingsInPot") {
                // implementar função para colocar porções na panela
            }
            else if (message.command == "returnChefToRest" && message.params.size() >= 1) {
                int chefId = std::stoi(message.params[0]);
                if (chefId >= 0 && chefId < chefs.size()) {
                    chefs[chefId].setState(3);
                    chefs[chefId].setTargetPot(-1);
                }
            }
            else if (message.command == "returnCustomerToRest" && message.params.size() >= 1) {
                int customerId = std::stoi(message.params[0]);
                if (customerId >= 0 && customerId < customers.size()) {
                    customers[customerId].setState(3);
                    customers[customerId].setTargetPot(-1);
                }
            }
            else if (message.command == "noFood" && message.params.size() >= 1) {
                int chefId = std::stoi(message.params[0]);
                if (chefId >= 0 && chefId < chefs.size()) {
                    chefs[chefId].setState(1);
                    chefs[chefId].setTargetPot(chefId);
                }
            }
            else if (message.command == "getFood" && message.params.size() >= 2) {
                int customerId = std::stoi(message.params[0]);
                int potId = std::stoi(message.params[1]);
                if (customerId >= 0 && customerId < customers.size()) {
                    customers[customerId].setTargetPot(potId);
                    customers[customerId].setState(1); // 1: indo para panela
                }
            }
        }
        
        // Pequena pausa para não sobrecarregar a CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void receiveSignals(std::map<std::string, int>& signals, std::vector<Customer>& customers, 
    std::vector<Chef>& chefs, sf::Clock& clock) {
    static MessageQueue messageQueue;

    // Inicia as duas threads
    std::thread receiver(receiverThread, std::ref(messageQueue));
    std::thread processor(processorThread, std::ref(messageQueue), std::ref(signals), 
            std::ref(customers), std::ref(chefs));

    // Desacopla as threads para executarem independentemente
    receiver.detach();
    processor.detach();

    // A função principal retorna imediatamente, permitindo que o programa continue
    std::cout << "Sistema de recepção de mensagens iniciado em threads separadas" << std::endl;
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

