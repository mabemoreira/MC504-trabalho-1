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
#include "../include/Customer.hpp"
#include "../include/Chef.hpp"

// Network
void receiveSignals(std::map<std::string, int> &signals, std::vector<Customer> &customers, std::vector<Chef> &chefs);

// Fontes
sf::Font loadFont(const std::string& fontPath);

// Customers
std::vector<std::string> getCustomerFiles();

#endif