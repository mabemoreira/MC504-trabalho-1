#ifndef CHEF_HPP
#define CHEF_HPP

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

namespace fs = std::filesystem;
using namespace std;

class Chef {
private:

public:
    Chef();
    ~Chef();

    // Métodos para manipular o estado do Chef
    void setMoving(bool moving);
    void setEating(bool eating);
    void setLeaving(bool leaving);
    void setTargetPot(int targetPot);
    void setEatingTimer(float eatingTimer);

    bool isMoving() const;
    bool isEating() const;
    bool isLeaving() const;
    int getTargetPot() const;
    float getEatingTimer() const;
    int getId() const;


};

#endif