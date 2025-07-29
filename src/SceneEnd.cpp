#include "SceneEnd.h"
#include "Game.h"
#include "SceneMain.h"
#include <string>
#include <iostream>

void SceneEnd::initialize() {
    if (!SDL_IsTextInputActive()) {
        SDL_StartTextInput();
    }
    if (!SDL_IsTextInputActive()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL", "Failed to start text input: s%", SDL_GetError());
    }
    // bgm
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (!bgm) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneEnd::update(float deltaTime) {
    timer += deltaTime;
    if (timer > 1.0f) {
        timer -= 1.0f;
    }
}

void SceneEnd::render() {
    if (isTyping) {
        renderPhase1();
    } else {
        renderPhase2();
    }
}

void SceneEnd::handleEvent(SDL_Event* event) {
    if (isTyping) {
        if (event->type == SDL_TEXTINPUT) {
            name += event->text.text;
        }
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
                isTyping = false;
                SDL_StopTextInput();
                if (name == "") {
                    name = "noname";
                }
                game.insertLeaderboard(name, game.getFinalScore());
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                removeLastUtf8Char(name);
            }
        }
    } else {
        if (event->key.keysym.scancode == SDL_SCANCODE_J) {
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
    }
}

void SceneEnd::cleanup() {
    if (!SDL_IsTextInputActive()) {
        SDL_StopTextInput();
    }
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}
void SceneEnd::removeLastUtf8Char(std::string& str) {
    if (str.empty()) {
        return;
    }
    char lastChar = str.back();
    if ((lastChar & 0b10000000) == 0b10000000) {
        str.pop_back();
        while ((str.back() & 0b11000000) != 0b11000000) {
            str.pop_back();
        }
    }
    str.pop_back();
}

void SceneEnd::renderPhase1() {
    auto score = game.getFinalScore();
    std::string scoreText = "你的最终得分是：" + std::to_string(score);
    std::string gameOver = "GAME OVER";
    std::string instructionText = "请输入你的名字，按回车确认：";
    game.renderTextCentered(scoreText, 0.1f, false);
    game.renderTextCentered(gameOver, 0.4f, true);
    game.renderTextCentered(instructionText, 0.6f, false);
    SDL_FPoint pos;
    if (name != "") {
        pos = game.renderTextCentered(name, 0.8f, false);
        if (timer < 0.5f) {
            game.renderTextPos("_", pos.x, pos.y, true);
        }
    } else {
        if (timer < 0.5f) {
            game.renderTextCentered("_", 0.8f, false);
        }
    }
    
}

void SceneEnd::renderPhase2() {
    game.renderTextCentered("得分排行榜", 0.1f, true);
    auto posY = game.getScreenHeight() * 0.2;
    auto i = 1;
    for (auto& player : game.getLeaderboard()) {
        std::string entryName = std::to_string(i) + "." + player.second;
        std::string score = std::to_string(player.first);
        game.renderTextPos(entryName, 100, static_cast<float>(posY), true);
        game.renderTextPos(score, 100, static_cast<float>(posY), false);
        posY += 45;
        i++;
    }
    if (timer < 0.5f) {
        game.renderTextCentered("按 J 重新开始游戏", 0.85f, false);
    }
}
