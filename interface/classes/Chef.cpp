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
#include "../include/Chef.hpp"

namespace fs = std::filesystem;
using namespace std;

Chef::Chef(int index_chef, sf::Font& font): id(index_chef), state(0), targetPot(-1), cookingTimer(0.0f), restTimer(0.0f)
{
    label.setFont(font);
    label.setString("Chef " + std::to_string(index_chef + 1));
    label.setCharacterSize(20);
    label.setFillColor(sf::Color::Black);
    label.setPosition(50, 50 + index_chef * 50); // Posição fixa para o label
}

// Chef::~Chef() {}