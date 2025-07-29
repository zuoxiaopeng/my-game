#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Scene.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

class ScenTitle : public Scene {
public:    
    virtual void initialize() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void handleEvent(SDL_Event* event) override;
    virtual void cleanup() override;

private:
    Mix_Music* bgm;
    float timer = 0.0f;
};


#endif