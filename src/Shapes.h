#pragma once

#include "SFML/Graphics.hpp"
#include <vector>
#include <Windows.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <random>

class Game
{
public:
    Game();
    void run();
private:
    void ProcessEvents();
    void update();
    void updateDown();
    void render();
    void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
    void initializeRects();
    void colorShapes(sf::Color color);
    void rotateCW();
    void rotateCCW();
    void GenerateShape();
    void checkScore();
    void moveDownwards();
    void generatePossible();
    void InitStrings();
    void Reset();
    bool checkPos(std::vector<std::pair<int, int>>& pos);
    bool shouldCopyShape(std::vector<std::pair<int, int>>& pos);
    void InitializeShapes();

    const int W = 11;
    const int H = 20;
    int score;
    std::vector<std::vector<std::pair<int, int>>> shapeList;
    float blockSize = 30.0f;
    bool movingLeft, movingUp, movingRight, movingDown;
    bool rotateLeft, rotateRight;
    const std::string font = "calibri.ttf";
    sf::RenderWindow myWindow;
    std::vector<std::pair<int, int>> generatedShape;
    std::vector<std::vector<sf::RectangleShape>> Rect; 
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;
    LARGE_INTEGER fpsInv;
    bool gameOverBool;
    bool gameRunning;
    sf::Font MyFont;
    sf::Text txt;
    sf::Text gameOver;
    sf::Text instructions;
    bool reset;
};