#define FPS_LIMIT 60

#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

#include "mingl/mingl.h"
#include "mingl/gui/text.h"
#include "mingl/shape/rectangle.h"
#include "mingl/shape/line.h"

using namespace std;

const int WIDTH = 640;
const int HEIGHT = 640;

nsGraphics::Vec2D player1Pos(20, 20);
nsGraphics::Vec2D player2Pos(620, 620);

std::vector<nsGraphics::Vec2D> obstacles = {
    nsGraphics::Vec2D(150, 150), nsGraphics::Vec2D(300, 300), nsGraphics::Vec2D(450, 450),
    nsGraphics::Vec2D(200, 400), nsGraphics::Vec2D(500, 100), nsGraphics::Vec2D(600, 300),
    nsGraphics::Vec2D(100, 500), nsGraphics::Vec2D(400, 200), nsGraphics::Vec2D(250, 250),
    nsGraphics::Vec2D(350, 550)
};

std::vector<nsGraphics::Vec2D> powerUps;

int player1Score = 0;
int player2Score = 0;
int level = 1;

float playerSpeed = 1.0f;

chrono::steady_clock::time_point gameEndTime;

bool collisionDetect()
{
    return player1Pos.getX() < player2Pos.getX() + 20 && player1Pos.getX() + 20 > player2Pos.getX() &&
           player1Pos.getY() < player2Pos.getY() + 20 && player1Pos.getY() + 20 > player2Pos.getY();
}

bool collisionObstacle(nsGraphics::Vec2D position)
{
    for (size_t i = 0; i < obstacles.size(); ++i)
    {
        if (position.getX() < obstacles[i].getX() + 20 && position.getX() + 20 > obstacles[i].getX() &&
            position.getY() < obstacles[i].getY() + 20 && position.getY() + 20 > obstacles[i].getY())
        {
            return true;
        }
    }
    return false;
}

bool collisionPowerUp(nsGraphics::Vec2D position)
{
    for (size_t i = 0; i < powerUps.size(); ++i)
    {
        if (position.getX() < powerUps[i].getX() + 20 && position.getX() + 20 > powerUps[i].getX() &&
            position.getY() < powerUps[i].getY() + 20 && position.getY() + 20 > powerUps[i].getY())
        {
            return true;
        }
    }
    return false;
}

void keyboard(MinGL &window)
{
    if (window.isPressed({'z', false}) && player1Pos.getY() > 0 && !collisionObstacle(player1Pos))
        player1Pos.setY(player1Pos.getY() - playerSpeed);
    if (window.isPressed({'s', false}) && player1Pos.getY() < HEIGHT - 20 && !collisionObstacle(player1Pos))
        player1Pos.setY(player1Pos.getY() + playerSpeed);
    if (window.isPressed({'q', false}) && player1Pos.getX() > 0 && !collisionObstacle(player1Pos))
        player1Pos.setX(player1Pos.getX() - playerSpeed);
    if (window.isPressed({'d', false}) && player1Pos.getX() < WIDTH - 20 && !collisionObstacle(player1Pos))
        player1Pos.setX(player1Pos.getX() + playerSpeed);

    if (window.isPressed({'u', false}) && player2Pos.getY() > 0 && !collisionObstacle(player2Pos))
        player2Pos.setY(player2Pos.getY() - playerSpeed);
    if (window.isPressed({'j', false}) && player2Pos.getY() < HEIGHT - 20 && !collisionObstacle(player2Pos))
        player2Pos.setY(player2Pos.getY() + playerSpeed);
    if (window.isPressed({'h', false}) && player2Pos.getX() > 0 && !collisionObstacle(player2Pos))
        player2Pos.setX(player2Pos.getX() - playerSpeed);
    if (window.isPressed({'k', false}) && player2Pos.getX() < WIDTH - 20 && !collisionObstacle(player2Pos))
        player2Pos.setX(player2Pos.getX() + playerSpeed);
}

void draw(MinGL &window, bool gameOver, int winner)
{
    string scoreText = "Player 1: " + to_string(player1Score) + "  Player 2: " + to_string(player2Score);
    string levelText = "Level: " + to_string(level);
    window << nsGui::Text(nsGraphics::Vec2D(10, 10), scoreText, nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);
    window << nsGui::Text(nsGraphics::Vec2D(500, 10), levelText, nsGraphics::KWhite, nsGui::GlutFont::BITMAP_9_BY_15);

    float progress = (float)(player1Score + player2Score) / (level * 10);
    window << nsShape::Rectangle(nsGraphics::Vec2D(10, 30), nsGraphics::Vec2D(10 + progress * 300, 50), nsGraphics::KGreen);

    if (gameOver)
    {
        string winnerText = "Player " + to_string(winner) + " Wins!";
        window << nsShape::Rectangle(nsGraphics::Vec2D(200, 250), nsGraphics::Vec2D(440, 320), nsGraphics::KYellow);
        window << nsGui::Text(nsGraphics::Vec2D(250, 270), winnerText, nsGraphics::KBlack, nsGui::GlutFont::BITMAP_9_BY_15);
    }
    else
    {
        window << nsShape::Rectangle(player1Pos, player1Pos + nsGraphics::Vec2D(20, 20), nsGraphics::KBlue);
        window << nsShape::Rectangle(player2Pos, player2Pos + nsGraphics::Vec2D(20, 20), nsGraphics::KMagenta);

        for (size_t i = 0; i < obstacles.size(); ++i)
        {
            window << nsShape::Rectangle(obstacles[i], obstacles[i] + nsGraphics::Vec2D(20, 20), nsGraphics::KRed);
        }

        for (size_t i = 0; i < powerUps.size(); ++i)
        {
            window << nsShape::Rectangle(powerUps[i], powerUps[i] + nsGraphics::Vec2D(15, 15), nsGraphics::KYellow);
        }
    }
}

void nextLevel()
{
    level++;
    obstacles.push_back(nsGraphics::Vec2D(rand() % WIDTH, rand() % HEIGHT));
    obstacles.push_back(nsGraphics::Vec2D(rand() % WIDTH, rand() % HEIGHT));

    if (rand() % 2 == 0)
    {
        powerUps.push_back(nsGraphics::Vec2D(rand() % WIDTH, rand() % HEIGHT));
    }

    playerSpeed += 0.5f;
}

int main()
{
    MinGL window("Two-player Game - Movement", nsGraphics::Vec2D(640, 640), nsGraphics::Vec2D(128, 128), nsGraphics::KBlack);
    window.initGlut();
    window.initGraphic();

    bool gameOver = false;
    int winner = 0;

    while (window.isOpen())
    {
        chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

        window.clearScreen();

        if (!gameOver)
        {
            keyboard(window);
        }

        if (collisionDetect() && !gameOver)
        {
            if (player1Pos.getX() < player2Pos.getX())
            {
                player1Score++;
                winner = 1;
            }
            else
            {
                player2Score++;
                winner = 2;
            }
            gameOver = true;
        }

        if (collisionPowerUp(player1Pos))
        {
            player1Score += 5;
            powerUps.clear();
        }
        if (collisionPowerUp(player2Pos))
        {
            player2Score += 5;
            powerUps.clear();
        }

        draw(window, gameOver, winner);

        if (window.isPressed({' ', false}) && gameOver)
        {
            player1Pos = nsGraphics::Vec2D(20, 20);
            player2Pos = nsGraphics::Vec2D(620, 620);
            gameOver = false;
            winner = 0;
            nextLevel();
        }

        window.finishFrame();
        this_thread::sleep_for(chrono::milliseconds(1000 / FPS_LIMIT));
    }

    return 0;
}
