#ifndef GAME_H
#define GAME_H
#include "Scene.h"
#include "Object.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <map>
#include <fstream>


class Game {
public:
    static Game& gameInstance() {
        static Game instance;
        return instance;
    }

    void initialize();
    void run();
    void changeScene(Scene* newScene);
    void cleanup();

    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();

    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }
    int getScreenWidth() { return screenWidth; }
    int getScreenHeight() { return screenHeight; }  
    int getFPS() { return FPS; } // Getter for FPS  

    // Background
    void backgroundUpdate(float deltaTime);
    void backgroundRender();

    // text render
    SDL_FPoint renderTextCentered(const std::string& text, float posY, bool isTitle);
    void renderTextPos(const std::string& text, float posX, float posY, bool isLeft);

    // score
    int getFinalScore() { return finalScore; }
    void setFinalScore(int score) { finalScore = score; }

    // leaderboard
    void insertLeaderboard(std::string& name, int score);
    std::multimap<int, std::string, std::greater<int>> getLeaderboard() { return leaderboard; }

    // save data
    void saveData();
    void loadData();



private:
    Game(); // Private constructor for singleton pattern
    ~Game(); // Private destructor for singleton pattern
    Game(const Game&) = delete; // Prevent copy construction
    Game& operator=(const Game&) = delete; // Prevent assignment

    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int screenWidth = 600;
    int screenHeight = 800;

    int FPS = 144; // Frames per second
    Uint32 frameTime; // Time taken for the current frame in milliseconds
    float deltaTime; // Time between frames in seconds

    // Background
    Background nearStars;
    Background farStars;

    // font
    TTF_Font* titleFont;
    TTF_Font* textFont;

    // score
    int finalScore = 0;

    // leaderboard
    std::multimap<int, std::string, std::greater<int>> leaderboard;

    // isFullscreen
    bool isFullscreen = false;
};


#endif // GAME_H