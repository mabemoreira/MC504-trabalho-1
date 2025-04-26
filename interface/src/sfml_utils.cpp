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
                              << signals["n_alunos"] << " alunos, "
                              << signals["n_chefs"] << " chefs e "
                              << signals["porcoes_por_aluno"] << " porções por aluno" << std::endl;
                }
                else if (command == "end")
                {
                    signals["end"] = 1;
                    // fazer algum comando caso a gente queria alguma tela após o fim do jogo
                }
                else if (command == "putServingsInPot")
                {
                    // implemetar função para colocar porções na panela
                }
                else if (command == "returnChefToRest")
                {
                    signals["returnChefToRest"] = std::stoi(parsedMessage[1]);
                }
                else if (command == "returnCostumerToRest")
                {
                    // returnCostumerToRest();
                }
                
                else if (command == "noFood")
                {
                }
                else if (command == "getFood")
                {
                }
                else
                    std::cerr << "Comando desconhecido: " << command << std::endl;
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
    std::cout << "Diretório atual: " << fs::current_path() << std::endl;
    std::vector<std::string> customerFiles;
    for (const auto &entry : fs::directory_iterator("../assets/customers"))
        if (entry.is_regular_file())
            customerFiles.push_back(entry.path().string());
    return customerFiles;
}

