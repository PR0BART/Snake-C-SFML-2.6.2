#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int GRID_SIZE = 40;
const int SNAKE_START_LENGTH = 5;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { MENU, PLAYING, GAME_OVER };

struct SnakeSegment {
    int x, y;
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::deque<SnakeSegment> snake;
    Direction direction;
    sf::Vector2i food;
    GameState gameState;
    int score;
    int highScore;
    sf::Font font;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text menuText;

    void placeFood() {
        food.x = (std::rand() % (WINDOW_WIDTH / GRID_SIZE)) * GRID_SIZE;
        food.y = (std::rand() % (WINDOW_HEIGHT / GRID_SIZE)) * GRID_SIZE;
    }

    void moveSnake() {
        SnakeSegment newHead = snake.front();
        switch (direction) {
        case UP: newHead.y -= GRID_SIZE; break;
        case DOWN: newHead.y += GRID_SIZE; break;
        case LEFT: newHead.x -= GRID_SIZE; break;
        case RIGHT: newHead.x += GRID_SIZE; break;
        }

        if (newHead.x < 0 || newHead.x >= WINDOW_WIDTH || newHead.y < 0 || newHead.y >= WINDOW_HEIGHT) {
            gameState = GAME_OVER;
            return;
        }

        for (const auto& segment : snake) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                gameState = GAME_OVER;
                return;
            }
        }

        snake.push_front(newHead);
        if (newHead.x == food.x && newHead.y == food.y) {
            placeFood();
            score++;
            scoreText.setString("Score: " + std::to_string(score));

            if (score > highScore) {
                highScore = score;
                saveHighScore();
                highScoreText.setString("Best Score: " + std::to_string(highScore));
            }
        }
        else {
            snake.pop_back();
        }
    }

    void handleInput() {
        if (gameState == MENU || gameState == GAME_OVER) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                resetGame();
            }
        }
        else if (gameState == PLAYING) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && direction != DOWN) direction = UP;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && direction != UP) direction = DOWN;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && direction != RIGHT) direction = LEFT;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && direction != LEFT) direction = RIGHT;
        }
    }

    void drawGame() {
        window.clear();
        if (gameState == MENU || gameState == GAME_OVER) {
            window.draw(menuText);
        }
        else {
            sf::RectangleShape segmentShape(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            segmentShape.setFillColor(sf::Color::Green);
            for (const auto& segment : snake) {
                segmentShape.setPosition(static_cast<float>(segment.x), static_cast<float>(segment.y));
                window.draw(segmentShape);
            }
            sf::RectangleShape foodShape(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            foodShape.setFillColor(sf::Color::Red);
            foodShape.setPosition(static_cast<float>(food.x), static_cast<float>(food.y));
            window.draw(foodShape);
            window.draw(scoreText);
            window.draw(highScoreText);
        }
        window.display();
    }

    void resetGame() {
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        snake.clear();
        direction = RIGHT;
        score = 0;
        gameState = PLAYING;
        for (int i = 0; i < SNAKE_START_LENGTH; ++i) {
            snake.push_back({ WINDOW_WIDTH / 2 - i * GRID_SIZE, WINDOW_HEIGHT / 2 });
        }
        placeFood();
        scoreText.setString("Score: " + std::to_string(score));
        highScoreText.setString("Best Score: " + std::to_string(highScore));
    }

    void loadHighScore() {
        std::ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
        else {
            highScore = 0;
        }
    }

    void saveHighScore() {
        std::ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game"), gameState(MENU), direction(RIGHT), score(0) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        loadHighScore();
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            std::cerr << "Could not load font!" << std::endl;
        }
        scoreText.setFont(font);
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        highScoreText.setFont(font);
        highScoreText.setCharacterSize(40);
        highScoreText.setFillColor(sf::Color::White);
        highScoreText.setPosition(WINDOW_WIDTH - 300, 10);

        menuText.setFont(font);
        menuText.setCharacterSize(60);
        menuText.setFillColor(sf::Color::White);
        menuText.setString("Press Enter to Start");
        menuText.setPosition((WINDOW_WIDTH - menuText.getLocalBounds().width) / 2, (WINDOW_HEIGHT - menuText.getLocalBounds().height) / 2);
    }

    void run() {
        sf::Clock clock;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            handleInput();
            if (gameState == PLAYING && clock.getElapsedTime().asMilliseconds() > 100) {
                moveSnake();
                clock.restart();
            }
            drawGame();
        }
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}