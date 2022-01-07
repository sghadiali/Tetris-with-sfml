#include "Shapes.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <streambuf>

Game::Game() : myWindow(sf::VideoMode(1000, 800), "Tetris Game"), Rect(W, std::vector<sf::RectangleShape>(H))
{
    movingUp = movingDown = movingLeft = movingRight = rotateLeft = rotateRight = gameRunning = reset = false;
    myWindow.setFramerateLimit(10);
    score = 0;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    InitStrings();
}

void Game::InitStrings()
{
    if (!MyFont.loadFromFile("calibri.ttf")) std::cout << "WE ARE HAVING AN ERROR!!\n";
    txt.setString("Press Space to Start!");
    txt.setFont(MyFont);
    txt.setPosition(sf::Vector2f(15.0f + W * (blockSize + 2.0f), 30.0f));
    gameOver.setString("GAME OVER!!!");
    gameOver.setFont(MyFont);
    gameOver.setPosition(sf::Vector2f(130.0f + W * (blockSize + 2.0f), 150.0f));
    gameOverBool = false;
    std::ifstream t("Instructions.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    instructions.setString(str);
    instructions.setFont(MyFont);
    instructions.setPosition(sf::Vector2f(15.0f + W * (blockSize + 2.0f), 250));
}

void Game::run()
{
    InitializeShapes();
    initializeRects();
    while (myWindow.isOpen())
    {
        ProcessEvents();        
        update();
        render();
    }
}

void Game::ProcessEvents()
{
    sf::Event event;
    while (myWindow.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::KeyPressed: handlePlayerInput(event.key.code, true); break;
        case sf::Event::KeyReleased: handlePlayerInput(event.key.code, false); break;
        case sf::Event::Closed: myWindow.close(); break;
        }
    }
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::W) movingUp = isPressed;
    else if (key == sf::Keyboard::S) movingDown = isPressed;
    else if (key == sf::Keyboard::A) movingLeft = isPressed;
    else if (key == sf::Keyboard::D) movingRight = isPressed;
    else if (key == sf::Keyboard::Left) rotateLeft = isPressed;
    else if (key == sf::Keyboard::Right) rotateRight = isPressed;
    else if (key == sf::Keyboard::Space) gameRunning = true;
    else if (key == sf::Keyboard::R) reset = true;
}

void Game::update()
{
    if (gameRunning)
    {
        updateDown();
        if (movingDown) moveDownwards();
        if (movingLeft)
        {
            bool shouldMove = true;
            for (int i = 0; i < generatedShape.size(); i++)
            {
                if (generatedShape[i].first < 1)
                {
                    shouldMove = false;
                    break;
                }
                else
                {
                    if (std::find(generatedShape.begin(), generatedShape.end(), std::pair<int, int>(generatedShape[i].first - 1, generatedShape[i].second)) == generatedShape.end())
                    {
                        if (Rect[generatedShape[i].first - 1][generatedShape[i].second].getFillColor() == sf::Color::Yellow)
                        {
                            shouldMove = false;
                            break;
                        }
                    }
                }
            }
            if (shouldMove)
                for (int i = 0; i < generatedShape.size(); i++)
                {
                    colorShapes(sf::Color::White);
                    generatedShape[i].first--;
                    colorShapes(sf::Color::Yellow);
                }
        }
        if (movingRight)
        {
            bool shouldMove = true;
            for (int i = 0; i < generatedShape.size(); i++)
            {
                if (generatedShape[i].first > W - 2)
                {
                    shouldMove = false;
                    break;
                }
                else
                {
                    if (std::find(generatedShape.begin(), generatedShape.end(), std::pair<int, int>(generatedShape[i].first + 1, generatedShape[i].second)) == generatedShape.end())
                    {
                        if (Rect[generatedShape[i].first + 1][generatedShape[i].second].getFillColor() == sf::Color::Yellow)
                        {
                            shouldMove = false;
                            break;
                        }
                    }
                }
            }
            if (shouldMove)
                for (int i = 0; i < generatedShape.size(); i++)
                {
                    colorShapes(sf::Color::White);
                    generatedShape[i].first++;
                    colorShapes(sf::Color::Yellow);
                }
        }
        if (rotateLeft)
        {
            colorShapes(sf::Color::White);
            rotateCCW();
            colorShapes(sf::Color::Yellow);
            rotateLeft = false;
        }
        if (rotateRight)
        {
            colorShapes(sf::Color::White);
            rotateCW();
            colorShapes(sf::Color::Yellow);
            rotateRight = false;
        }
        if (reset) Reset();
    }
}

void Game::render()
{
    myWindow.clear();
    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            myWindow.draw(Rect[i][j]);
        }
    }
    myWindow.draw(txt);
    if (gameOverBool) myWindow.draw(gameOver);
    myWindow.draw(instructions);
    myWindow.display();
}
void Game::initializeRects()
{
    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            Rect[i][j].setSize(sf::Vector2f(blockSize, blockSize));
            Rect[i][j].setFillColor(sf::Color::White);
            Rect[i][j].setPosition(sf::Vector2f(10.0f + i * (blockSize + 2.0f), 10.0f + j * (blockSize + 2.0f)));
        }
    }
    GenerateShape();
    colorShapes(sf::Color::Yellow);
}

void Game::InitializeShapes()
{
    int pos = W / 2;
    shapeList.resize(7);
    shapeList[0].push_back(std::pair<int, int>(pos, 0));
    shapeList[0].push_back(std::pair<int, int>(pos, 1));
    shapeList[0].push_back(std::pair<int, int>(pos - 1, 1));
    shapeList[0].push_back(std::pair<int, int>(pos + 1, 1));

    shapeList[1].push_back(std::pair<int, int>(pos, 1));
    shapeList[1].push_back(std::pair<int, int>(pos, 2));
    shapeList[1].push_back(std::pair<int, int>(pos, 3));
    shapeList[1].push_back(std::pair<int, int>(pos, 0));

    shapeList[2].push_back(std::pair<int, int>(pos, 0));
    shapeList[2].push_back(std::pair<int, int>(pos, 1));
    shapeList[2].push_back(std::pair<int, int>(pos, 2));
    shapeList[2].push_back(std::pair<int, int>(pos + 1, 2));

    shapeList[3].push_back(std::pair<int, int>(pos, 0));
    shapeList[3].push_back(std::pair<int, int>(pos, 1));
    shapeList[3].push_back(std::pair<int, int>(pos, 2));
    shapeList[3].push_back(std::pair<int, int>(pos - 1, 2));

    shapeList[4].push_back(std::pair<int, int>(pos, 0));
    shapeList[4].push_back(std::pair<int, int>(pos, 1));
    shapeList[4].push_back(std::pair<int, int>(pos + 1, 0));
    shapeList[4].push_back(std::pair<int, int>(pos + 1, 1));

    shapeList[5].push_back(std::pair<int, int>(pos, 0));
    shapeList[5].push_back(std::pair<int, int>(pos - 1, 0));
    shapeList[5].push_back(std::pair<int, int>(pos, 1));
    shapeList[5].push_back(std::pair<int, int>(pos + 1, 1));

    shapeList[6].push_back(std::pair<int, int>(pos, 0));
    shapeList[6].push_back(std::pair<int, int>(pos, 1));
    shapeList[6].push_back(std::pair<int, int>(pos + 1, 0));
    shapeList[6].push_back(std::pair<int, int>(pos - 1, 1));
}

void Game::GenerateShape()
{
    checkScore();
    generatePossible();

    srand(time(NULL));
    int r = rand()%7;
    generatedShape = shapeList[r];
}

void Game::colorShapes(sf::Color color)
{
    for (int i = 0; i < generatedShape.size(); i++)
    {
        Rect[generatedShape[i].first][generatedShape[i].second].setFillColor(color);
    }
}

// This rotate logic could be combined
void Game::rotateCW()
{
    std::pair<int, int> pivot = generatedShape[1];
    std::vector<std::pair<int, int>> myShape;
    for (int i = 0; i < generatedShape.size(); i++)
    {
        int x =  generatedShape[i].first - pivot.first;
        int y = generatedShape[i].second - pivot.second;
        int x_ = -y;
        int y_ = x;
        x = x_ + pivot.first;
        y = pivot.second + y_;
        myShape.push_back(std::pair<int, int>(x, y));
        
    }
    checkPos(myShape);
    checkPos(myShape);
    checkPos(myShape);
    if (shouldCopyShape(myShape))
        generatedShape = myShape;
}
void Game::rotateCCW()
{
    std::pair<int, int> pivot = generatedShape[1];
    std::vector<std::pair<int, int>> myShape;
    for (int i = 0; i < generatedShape.size(); i++)
    {
        int x = generatedShape[i].first - pivot.first;
        int y = generatedShape[i].second - pivot.second;
        int x_ = y;
        int y_ = -x;
        x = x_ + pivot.first;
        y = pivot.second + y_;
        myShape.push_back(std::pair<int, int>(x, y));
        
    }
    checkPos(myShape);
    checkPos(myShape);
    
    if (checkPos(myShape) && shouldCopyShape(myShape))
        generatedShape = myShape;
}

//checkes whether new position generated is out of bound
bool Game::checkPos(std::vector<std::pair<int, int>>& pos)
{
    bool leftErr = false;
    bool rightErr = false;
    
    for (int i = 0; i < pos.size(); i++)
    {
        if (pos[i].first < 0)
        {
            leftErr = true;
            break;
        }
        if (pos[i].first >= W)
        {
            rightErr = true;
            break;
        }
    }
    if (leftErr) for (int i = 0; i < pos.size(); i++) pos[i].first++;
    if (rightErr)
    {
        for (int i = 0; i < pos.size(); i++) pos[i].first--;
    }
    bool horizontal = true;
    for (int i = 0; i < pos.size(); i++)
    {
        if (pos[i].second < 0 || pos[i].second >= H)
        {
            horizontal = false;
            break;
        }
    }
    return horizontal;
}

//checks whether new position generated after roating can be rorated or not
bool Game::shouldCopyShape(std::vector<std::pair<int, int>>& pos)
{
    for (int i = 0; i < pos.size(); i++)
    {
        if (Rect[pos[i].first][pos[i].second].getFillColor() != sf::Color::White)
        {
            return false;
        }
    }
    return true;
}

//This is handling multiple logic
void Game::checkScore()
{
    std::vector<int> ind;

    for (int j = H - 1; j >= 0; j--)
    {
        bool isLineFull = true;
        for (int i = 0; i < W; i++)
        {
            if (Rect[i][j].getFillColor() == sf::Color::White)
            {
                isLineFull = false;
                break;
            }
        }
        if (isLineFull) ind.push_back(j);
    }
    for (int k = 0; k < ind.size(); k++)
    {
        score++;
        int J = ind[k];
        for (int j = J; j > 0; j--)
        {
            for (int i = 0; i < W; i++)
            {
                Rect[i][j].setFillColor(Rect[i][j - 1].getFillColor());
            }
        }
        for (int i = 0; i < W; i++)
        {
            Rect[i][0].setFillColor(sf::Color::White);
        }
    }
    if (gameRunning)
    {
        sf::String sc("SCORE : " + std::to_string(score));
        txt.setString(sc);
    }
    
}

void Game::moveDownwards()
{
    bool shouldMove = true;
    for (int i = 0; i < generatedShape.size(); i++)
    {
        if (generatedShape[i].second > H - 2)
        {
            shouldMove = false;
            break;
        }
        else
        {
            if (std::find(generatedShape.begin(), generatedShape.end(), std::pair<int, int>(generatedShape[i].first, generatedShape[i].second + 1)) == generatedShape.end())
            {
                if (Rect[generatedShape[i].first][generatedShape[i].second + 1].getFillColor() == sf::Color::Yellow)
                {
                    shouldMove = false;
                    break;
                }
            }
        }
    }
    if (shouldMove) for (int i = 0; i < generatedShape.size(); i++)
    {
        colorShapes(sf::Color::White);
        generatedShape[i].second++;
        colorShapes(sf::Color::Yellow);
    }
    else GenerateShape();
}

void Game::updateDown()
{
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

    ElapsedMicroseconds.QuadPart *= 10;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    if (ElapsedMicroseconds.QuadPart >= 3)
    {
        moveDownwards();
        QueryPerformanceCounter(&StartingTime);
    }
}

void Game::generatePossible()
{
    int pos = W / 2;
    for (int j = pos - 1; j < pos + 2; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            if (Rect[j][i].getFillColor() != sf::Color::White)
            {
                gameRunning = false;
                gameOverBool = true;
                break;
            }
        }
    }    
}

void Game::Reset()
{
    initializeRects();
    score = 0;
    reset = false;
    gameRunning = true;
    gameOverBool = false;
    txt.setString("Press Space to Start!");
}