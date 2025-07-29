#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>

void Game::insertLeaderboard(std::string& name, int score) {
    leaderboard.insert({ score, name });
    if (leaderboard.size() > 8) {
        leaderboard.erase(--leaderboard.end());
    }
}

void Game::saveData() {
    std::ofstream file("assets/save.dat");
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open save file");
        return;
    }
    for (auto& entry : leaderboard) {
        file << entry.first << " " << entry.second << std::endl;
    }
}

void Game::loadData() {
    std::ifstream file("assets/save.dat");
    if (!file.is_open()) {
        SDL_Log("Failed to open save file");
        return;
    }
    leaderboard.clear();
    int score;
    std::string name;
    while (file >> score >> name) {
        insertLeaderboard(name, score);
    }
}

Game::Game() {
}

Game::~Game() {
    saveData();
    cleanup();
}

void Game::initialize() {
    // Set the frame time based on the desired FPS
    frameTime = 1000 / FPS; // Convert FPS to milliseconds per frame
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s", SDL_GetError());
        isRunning = false;
    }

    // Create window
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow Error: %s", SDL_GetError());
        isRunning = false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer Error: %s", SDL_GetError());
        isRunning = false;
    }
    SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);

    // Initialize the SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
        isRunning = false;
    }

    // initialize the SDL_mixer
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_image Error: %s", Mix_GetError());
        isRunning = false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not open audio! SDL_image Error: %s", Mix_GetError());
        isRunning = false;
    }

    Mix_AllocateChannels(32);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 8);

    // init TTF
    if (TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
        isRunning = false;
    }

    // Background
    nearStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-A.png");
    SDL_QueryTexture(nearStars.texture, nullptr, nullptr, &nearStars.width, &nearStars.height);
    nearStars.width /= 2;
    nearStars.height /= 2;

    farStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-B.png");
    SDL_QueryTexture(farStars.texture, nullptr, nullptr, &farStars.width, &farStars.height);
    farStars.width /= 2;
    farStars.height /= 2;
    farStars.speed = 20;

    //init font
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);

    if (titleFont == nullptr || textFont == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpentFont:%s", SDL_GetError());
        isRunning = false;
    }
    
    // load date
    loadData();

    // Initialize with a default scene
    currentScene = new ScenTitle(); 
    currentScene->initialize();
}

void Game::run() {
    while (isRunning) {
        // Calculate delta time
        auto currentTime = SDL_GetTicks();
        
        SDL_Event* event = new SDL_Event();
        handleEvent(event);
        update(deltaTime);
        render();
        
        auto endTime = SDL_GetTicks();
        auto diff = endTime - currentTime; // Calculate frame time
        
        if (diff < frameTime) {
            SDL_Delay(frameTime - diff); // Delay to maintain FPS
            deltaTime = static_cast<float>(frameTime) / 1000.0f; // Convert to seconds
        } else {
            deltaTime = static_cast<float>(diff) / 1000.0f; // Convert to seconds
        }
    }
}

void Game::changeScene(Scene *newScene) {
    if (currentScene != nullptr) {
        currentScene->cleanup();
        delete currentScene;
    }
    currentScene = newScene;
    currentScene->initialize();
    
}

SDL_FPoint Game::renderTextCentered(const std::string& text, float posY, bool isTitle) {
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface;
    if (isTitle) {
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    } else {
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
    int y = static_cast<int>((getScreenHeight() * posY) - surface->h);
    SDL_Rect rect = {
        getScreenWidth() / 2 - surface->w / 2,
        y,
        surface->w,
        surface->h
    };
    SDL_RenderCopy(getRenderer(), texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return { static_cast<float>(rect.x + rect.w) , static_cast<float>(rect.y) };
}

void Game::renderTextPos(const std::string& text, float posX, float posY, bool isLeft) {
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
    SDL_Rect rect;
    if (isLeft) {
        rect = { static_cast<int>(posX), static_cast<int>(posY), surface->w, surface->h };
    } else {
        rect = { static_cast<int>(getScreenWidth() - posX - surface->w), static_cast<int>(posY), surface->w, surface->h };
    }
    SDL_RenderCopy(getRenderer(), texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::cleanup() {
    if (currentScene != nullptr) {
        currentScene->cleanup();
        delete currentScene;
    }
   
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();

    // Background
    if (nearStars.texture != nullptr) {
        SDL_DestroyTexture(nearStars.texture);
        nearStars.texture = nullptr;
    }
    if (farStars.texture != nullptr) {
        SDL_DestroyTexture(farStars.texture);
        farStars.texture = nullptr;
    }
}

void Game::handleEvent(SDL_Event *event) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            isRunning = false;
        }
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_F4) {
                isFullscreen = !isFullscreen;
                if (isFullscreen) {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                } else {
                    SDL_SetWindowFullscreen(window, 0);
                }
            }
        }
        currentScene->handleEvent(event);
    }
}

void Game::update(float dt) {
    backgroundUpdate(dt);
    currentScene->update(dt);
}

void Game::render() {
    // Clear the screen
    SDL_RenderClear(renderer);

    // Render background
    backgroundRender();
    
    // Render the current scene
    currentScene->render();
    
    // update the screen
    SDL_RenderPresent(renderer);
}

// Background
void Game::backgroundUpdate(float dt) {
    nearStars.offset += nearStars.speed * dt;
    if (nearStars.offset >= 0) {
        nearStars.offset -= nearStars.height;
    }

    farStars.offset += farStars.speed * dt;
    if (farStars.offset >= 0) {
        farStars.offset -= farStars.height;
    }
}

void Game::backgroundRender() {
    for (int posY = static_cast<int>(farStars.offset); posY < getScreenHeight(); posY += farStars.height) {
        for (int posX = 0; posX < getScreenWidth(); posX += farStars.width) {
            SDL_Rect rect = {
                posX,
                posY,
                farStars.width,
                farStars.height
            };
            SDL_RenderCopy(getRenderer(), farStars.texture, nullptr, &rect);
        }
    }

    for (int posY = static_cast<int>(nearStars.offset); posY < getScreenHeight(); posY += nearStars.height) {
        for (int posX = 0; posX < getScreenWidth(); posX += nearStars.width) {
            SDL_Rect rect = {
                posX,
                posY,
                nearStars.width,
                nearStars.height
            };
            SDL_RenderCopy(getRenderer(), nearStars.texture, nullptr, &rect);
        }
    }
}


