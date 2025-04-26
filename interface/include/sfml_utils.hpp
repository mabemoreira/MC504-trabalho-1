#ifndef SFML_UTILS_HPP
#define SFML_UTILS_HPP

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

// Network
void receiveSignals(std::map<std::string, int>& signals);

// Fontes
sf::Font loadFont(const std::string& fontPath);

// Customers
std::vector<std::string> getCustomerFiles();

#endif