#pragma once

#include "Scene.h"
#include <string>
#include <SDL_mixer.h>

class SceneEnd : public Scene
{
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void cleanup() override;
    void removeLastUtf8Char(std::string& str);

private:
    std::string name;
    bool isTyping = true;
    float timer;
    Mix_Music* bgm;

    void renderPhase1();
    void renderPhase2();
};
