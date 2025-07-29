#ifndef SCENE_H
#define SCENE_H
#include <SDL.h>

class Game;

class Scene {
public:
    Scene();
    virtual ~Scene() = default;

    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
    virtual void cleanup() = 0;
protected:
    Game& game;
};

#endif // SCENE_H