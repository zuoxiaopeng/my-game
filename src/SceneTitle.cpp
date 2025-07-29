#include "SceneTitle.h"
#include "Game.h"
#include "SceneMain.h"
#include <string>



void ScenTitle::initialize(){
    SDL_StopTextInput(); // Stop text input if any
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void ScenTitle::update(float deltaTime){
    timer += deltaTime;
    if (timer > 1.0f) {
        timer -= 1.0f;
    }
}

void ScenTitle::render(){
    std::string titleText = "SDL太空战机";
    game.renderTextCentered(titleText, 0.4f, true);
    std::string instructions = "按 J 开始游戏";
    if (timer < 0.5f) {
        game.renderTextCentered(instructions, 0.8f, false);
    }
}

void ScenTitle::handleEvent(SDL_Event *event){
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.scancode == SDL_SCANCODE_J) {
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
    }
}

void ScenTitle::cleanup() {
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}
