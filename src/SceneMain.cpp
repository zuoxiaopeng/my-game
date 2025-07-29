#include "SceneMain.h"
#include "Game.h"
#include "SceneEnd.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

void SceneMain::initialize()
{
    SDL_StopTextInput(); // Stop text input if any

    // Initialize random number generator
    std::random_device rd; // Random device for seeding
    gen = std::mt19937(rd()); // Mersenne Twister random number generator
    dis = std::uniform_real_distribution<float>(0.0f, 1.0f); // Uniform distribution between 0 and 1

    // Initialization code for the main scene goes here
    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    SDL_QueryTexture(player.texture, nullptr, nullptr, &player.width, &player.height);
    player.width /= 4;
    player.height /= 4;
    player.position.x = static_cast<float>(game.getScreenWidth()) / 2 - static_cast<float>(player.width) / 2;
    player.position.y = static_cast<float>(game.getScreenHeight()) - static_cast<float>(player.height);

    // Initialize projectile template
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, nullptr, nullptr, &projectilePlayerTemplate.width, &projectilePlayerTemplate.height);
    projectilePlayerTemplate.width /= 4;
    projectilePlayerTemplate.height /= 4;

    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, nullptr, nullptr, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;

    // Initialize enemy projectile template
    projectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-2.png");
    SDL_QueryTexture(projectileEnemyTemplate.texture, nullptr, nullptr, &projectileEnemyTemplate.width, &projectileEnemyTemplate.height);
    projectileEnemyTemplate.width /= 4;
    projectileEnemyTemplate.height /= 4;

    // Initialize explosion template
    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, nullptr, nullptr, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totalFrames = explosionTemplate.width / explosionTemplate.height; // Assuming the explosion texture is a sprite sheet
    explosionTemplate.width = explosionTemplate.height;

    // Initialize music
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");

    // UI init
    UIHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png"); // init Health
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
}

void SceneMain::update(float deltaTime) {
    // Update logic for the main scene goes here
    keybordControl(deltaTime); // Call the keyboard control function to update player position
    projectileUpdate(deltaTime); // Update projectiles
    
    spawnEnemy(); // Spawn enemies
    enemyUpdate(deltaTime); // Update enemies
    enemyProjectileUpdate(deltaTime); // Update enemy projectiles
    explosionUpdate(deltaTime); // Update explosions
    if (!isDead) {
        playerUpdate(deltaTime); // Update player
    }
    itemUpdate(deltaTime);

    // end delay
    if (isDead) {
        changeSceneDelayed(deltaTime, 3);
    }
}

void SceneMain::render() {
    // rendering enemy projectile
    enemyProjectileRender();

    // rendering player
    if (!isDead){
        // rendering projectile
        projectileRender(); 
        SDL_Rect playerRect = { static_cast<int>(player.position.x), 
                            static_cast<int>(player.position.y), 
                            player.width, 
                            player.height };
        SDL_RenderCopy(game.getRenderer(), player.texture, nullptr, &playerRect);
    }

    // rendering enemy
    enemyRender();

    // rendering explosion
    explosionRender();

    // rendering itme
    itemRender();

    // rendering UI
    renderUI();
}

void SceneMain::handleEvent(SDL_Event *event) {
    (void) event; // Suppress unused parameter warning
}

void SceneMain::cleanup() {
    // Cleanup player
    // if (isDead) {
    //     SDL_Delay(2000); // Delay for 2 seconds before cleanup
    //     delete &player; // Free the memory
    //     player.texture = nullptr; // Set texture to nullptr
    // }

    // Cleanup code for the main scene goes here
    if (player.texture) {
        SDL_DestroyTexture(player.texture);
        player.texture = nullptr;
    }

    // Cleanup projectiles
    for (auto projectile : projectilePlayerList) {
        if (projectile->texture) {
            SDL_DestroyTexture(projectile->texture);
            projectile->texture = nullptr;
        }
        delete projectile; // Free the memory
    }
    projectilePlayerList.clear(); // Clear the list of projectiles
    if (projectilePlayerTemplate.texture) {
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
        projectilePlayerTemplate.texture = nullptr;
    }

    // Cleanup enemies
    for (auto enemy : enemyList) {
        if (enemy->texture) {
            SDL_DestroyTexture(enemy->texture);
            enemy->texture = nullptr;
        }
        delete enemy; // Free the memory
    }
    enemyList.clear(); // Clear the list of enemies
    if (enemyTemplate.texture) {
        SDL_DestroyTexture(enemyTemplate.texture);
        enemyTemplate.texture = nullptr;
    }

    // Cleanup enemy projectiles
    for (auto projectile : projectileEnemyList) {
        if (projectile->texture) {
            SDL_DestroyTexture(projectile->texture);
            projectile->texture = nullptr;
        }
        delete projectile; // Free the memory
    }
    projectileEnemyList.clear(); // Clear the list of enemy projectiles
    if (projectileEnemyTemplate.texture) {
        SDL_DestroyTexture(projectileEnemyTemplate.texture);
        projectileEnemyTemplate.texture = nullptr;
    }

    // Cleanup explosions
    for (auto explosion : explosionList) {
        if (explosion->texture) {
            SDL_DestroyTexture(explosion->texture);
            explosion->texture = nullptr;
        }
        delete explosion; // Free the memory
    }
    explosionList.clear(); // Clear the list of explosions
    if (explosionTemplate.texture) {
        SDL_DestroyTexture(explosionTemplate.texture);
        explosionTemplate.texture = nullptr;
    }

    // Cleanup items
    for (auto item : itemList) {
        if (item->texture != nullptr) {
            SDL_DestroyTexture(item->texture);
        }
    }

    // Cleanup music
    for (auto& sound : sounds) {
        if (sound.second != nullptr) {
            Mix_FreeChunk(sound.second);
        }
    }
    sounds.clear();
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }

    // UI clear
    if (UIHealth != nullptr) {
        SDL_DestroyTexture(UIHealth);
    }

    if (scoreFont != nullptr) {
        TTF_CloseFont(scoreFont);
    }

}

void SceneMain::keybordControl(float deltaTime) {
    auto keybordState = SDL_GetKeyboardState(nullptr);

    if (keybordState[SDL_SCANCODE_A]) {
        player.position.x -= player.speed * deltaTime;
    }
    if (keybordState[SDL_SCANCODE_D]) {
        player.position.x += player.speed * deltaTime;
    }
    if (keybordState[SDL_SCANCODE_W]) {
        player.position.y -= player.speed * deltaTime;
    }
    if (keybordState[SDL_SCANCODE_S]) {
        player.position.y += player.speed * deltaTime;
    }



    if (player.position.x < 0) {
        player.position.x = 0;
    }
    if (player.position.x + player.width > game.getScreenWidth()) {
        player.position.x = static_cast<float>(game.getScreenWidth()) - player.width;
    }
    if (player.position.y < 0) {
        player.position.y = 0;
    }
    if (player.position.y + player.height > game.getScreenHeight()) {
        player.position.y = static_cast<float>(game.getScreenHeight()) - player.height;
    }

    // shoot
    if (keybordState[SDL_SCANCODE_J]) {
        // shoot logic here
        Uint32 currentTime = SDL_GetTicks(); // Get the current time in milliseconds
        if (currentTime - player.lastShootTime > player.cooldown) { // Check if cooldown has passed
            palayerShoot(); // Call the shoot function
            player.lastShootTime = currentTime; // Update the last shoot time
        }
    }

}

void SceneMain::playerUpdate(float) {
    if (isDead) {
        return; // If player is dead, skip update
    }
    if (player.currentHealth <= 0) {
        Mix_PlayChannel(-1, sounds["player_explode"], 0);
        playerExplode(player.position); // Call the explode function
        isDead = true; // Set player dead flag
        game.setFinalScore(score);
        return;
    }
    for (auto it = enemyList.begin(); it != enemyList.end();) {
        auto enemy = *it;
        SDL_Rect enemyRect = { static_cast<int>(enemy->position.x), 
                                static_cast<int>(enemy->position.y), 
                                enemy->width, 
                                enemy->height };
        SDL_Rect playerRect = { static_cast<int>(player.position.x),
                                static_cast<int>(player.position.y), 
                                player.width, 
                                player.height };
        if (SDL_HasIntersection(&enemyRect, &playerRect)) {
            player.currentHealth -= 1; // Reduce player health
            enemy->currentHealth -= 1; // Reduce enemy health
        } 
        ++it; // Move to the next enemy
    }
   
}

void SceneMain::playerExplode(SDL_FPoint position) {
    auto currentTime = SDL_GetTicks(); // Get the current time in milliseconds
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = position.x + player.width / 2 - explosion->width / 2; // Center the explosion on the player
    explosion->position.y = position.y + player.height / 2 - explosion->height / 2; // Center the explosion on the player
    explosion->startTime = currentTime; // Set the start time for the explosion
    explosionList.push_back(explosion); // Add the explosion to the list
}

void SceneMain::palayerShoot() {
    if (isDead) {
        return;
    }
    auto projectile = new projectilePlayer(projectilePlayerTemplate);
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y - projectile->height;
    projectilePlayerList.push_back(projectile);
    // sounds
    Mix_PlayChannel(0, sounds["player_shoot"], 0);
}

void SceneMain::projectileUpdate(float deltaTime) {
    auto magin = 32;
    for (auto it = projectilePlayerList.begin(); it != projectilePlayerList.end();) {
        auto projectile = *it;
        projectile->position.y -= projectile->speed * deltaTime;
        if (projectile->position.y + magin < 0) {
            it = projectilePlayerList.erase(it); // Remove the projectile if it goes off-screen
            delete projectile; // Free the memory
        } else {
            bool hit = false;
            for (auto enemy : enemyList) {
                SDL_Rect projectilePlayerRect = { static_cast<int>(projectile->position.x), 
                                     static_cast<int>(projectile->position.y), 
                                     projectile->width, 
                                     projectile->height };
                SDL_Rect enemyRect = { static_cast<int>(enemy->position.x), 
                                        static_cast<int>(enemy->position.y), 
                                        enemy->width, 
                                        enemy->height };
                if (SDL_HasIntersection(&projectilePlayerRect, &enemyRect)) {   
                    enemy->currentHealth -= projectile->damage; // Reduce enemy health
                    it = projectilePlayerList.erase(it); // Remove the projectile
                    delete projectile; // Free the memory
                    hit = true; // Set hit to true
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                    break;
                }
            }
            if (!hit) {
                ++it; // Move to the next projectile
            }
        }
    }
}

void SceneMain::projectileRender() {
    for (auto projectile : projectilePlayerList) {
        SDL_Rect projectilePlayerRect = { static_cast<int>(projectile->position.x), 
                                     static_cast<int>(projectile->position.y), 
                                     projectile->width, 
                                     projectile->height };
        SDL_RenderCopy(game.getRenderer(), projectile->texture, nullptr, &projectilePlayerRect);
    }
}

void SceneMain::enemyUpdate(float deltaTime) {
    auto magin = 0;
    for (auto it = enemyList.begin(); it != enemyList.end();) {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        auto currentTime = SDL_GetTicks(); // Get the current time in milliseconds
        if ((currentTime - enemy->lastShootTime > enemy->cooldown) && (player.position.y > enemy->position.y)) { // Check if cooldown has passed
            enemyShoot(enemy); // Call the shoot function
            enemy->lastShootTime = currentTime; // Update the last shoot time
        }
        if (enemy->position.y > game.getScreenHeight() + magin) {
            it = enemyList.erase(it); // Remove the enemy if it goes off-screen
            delete enemy; // Free the memory
        } else if (enemy->currentHealth <= 0) {
            it = enemyList.erase(it); // Remove the enemy if health is 0
            enemyExplode(enemy); // Call the explode function
        } else {
            ++it; // Move to the next enemy
        }
    }
}

void SceneMain::enemyRender() {
    for (auto enemy : enemyList) {
        SDL_Rect enemyRect = { static_cast<int>(enemy->position.x), 
                                static_cast<int>(enemy->position.y), 
                                enemy->width, 
                                enemy->height };
        SDL_RenderCopy(game.getRenderer(), enemy->texture, nullptr, &enemyRect);
    }
}

void SceneMain::spawnEnemy() {
    if (dis(gen) > 1.0f / game.getFPS()) { // Spawn enemy with a certain probability
        return; // Limit the number of enemies to 10
    }
    auto enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getScreenWidth() - enemy->width); // Random x position
    enemy->position.y = - static_cast<float>(enemy->height); // Start above the screen
    enemyList.push_back(enemy); // Add the enemy to the list
}

void SceneMain::enemyShoot(Enemy* enemy) {
    if (isDead) {
        return; // If player is dead, skip shooting
    }

    auto projectile = new projectileEnemy(projectileEnemyTemplate);
    projectile->position.x = enemy->position.x + enemy->width / 2 - projectile->width / 2; // Center the projectile on the enemy
    projectile->position.y = enemy->position.y + enemy->height; // Start below the enemy
    projectile->direction = getDirection(enemy); // Get the direction towards the player
    projectileEnemyList.push_back(projectile); // Add the projectile to the list
    Mix_PlayChannel(-1, sounds["enemy_shoot"], 0); //Add sounds
}

void SceneMain::enemyProjectileUpdate(float deltaTime) {
    auto magin = 32;
    for (auto it = projectileEnemyList.begin(); it != projectileEnemyList.end();) {
        auto projectile = *it;
        projectile->position.x += projectile->direction.x * projectile->speed * deltaTime; // Update x position
        projectile->position.y += projectile->direction.y * projectile->speed * deltaTime; // Update y position
        if (projectile->position.y > game.getScreenHeight() + magin
            || projectile->position.x < -magin
            || projectile->position.x > game.getScreenWidth() + magin
            || projectile->position.y < -magin) {
            it = projectileEnemyList.erase(it); // Remove the projectile if it goes off-screen
            delete projectile; // Free the memory
        } else {
            bool hit = false;
            SDL_Rect projectileEnemyRect = { static_cast<int>(projectile->position.x), 
                                     static_cast<int>(projectile->position.y), 
                                     projectile->width, 
                                     projectile->height };
            SDL_Rect playerRect = { static_cast<int>(player.position.x), 
                                    static_cast<int>(player.position.y), 
                                    player.width, 
                                    player.height };
            if (SDL_HasIntersection(&projectileEnemyRect, &playerRect)) {
                player.currentHealth -= projectile->damage; // Reduce player health
                it = projectileEnemyList.erase(it); // Remove the projectile
                delete projectile; // Free the memory
                hit = true; // Set hit to true
                Mix_PlayChannel(-1, sounds["player_shoot"], 0);
            }
            if (!hit) {
                ++it; // Move to the next projectile
            }

        }
    }
}

void SceneMain::enemyProjectileRender() {
    for (auto projectile : projectileEnemyList) {
        SDL_Rect projectileEnemyRect = { static_cast<int>(projectile->position.x), 
                                     static_cast<int>(projectile->position.y), 
                                     projectile->width, 
                                     projectile->height };
        auto angle = atan2f(projectile->direction.y, projectile->direction.x) * 180.0f / M_PI - 90; // Calculate the angle in degrees
        SDL_RenderCopyEx(game.getRenderer(), projectile->texture, nullptr, &projectileEnemyRect, angle, nullptr, SDL_FLIP_NONE); // Render the projectile with vertical flip
    }
}

SDL_FPoint SceneMain::getDirection(Enemy *enemy) {
    float x = player.position.x - enemy->position.x;
    float y = player.position.y - enemy->position.y;
    float length = sqrtf(x * x + y * y);
    if (length > 0) {
        x /= length; // Normalize the x component
        y /= length; // Normalize the y component
    }
    SDL_FPoint direction = { x, y }; // Create a normalized direction vector
    return direction; // Return the direction vector
}

void SceneMain::enemyExplode(Enemy *enemy) {
    auto currentTime = SDL_GetTicks(); // Get the current time in milliseconds
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2; // Center the explosion on the enemy
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2; // Center the explosion on the enemy
    explosion->startTime = currentTime; // Set the start time for the explosion
    explosionList.push_back(explosion); // Add the explosion to the list
    Mix_PlayChannel(-1, sounds["enemy_explode"], 0);
    if (dis(gen) < 0.5f) {
        dropItem(enemy);
    }
    score += 10;
    delete enemy;
}

void SceneMain::explosionUpdate(float) {
    for (auto it = explosionList.begin(); it != explosionList.end();) {
        auto explosion = *it;
        auto currentTime = SDL_GetTicks(); // Get the current time in milliseconds
        explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000; // Calculate the current frame based on time
        if (explosion->currentFrame >= explosion->totalFrames) {
            it = explosionList.erase(it); // Remove the explosion if all frames are done
            delete explosion; // Free the memory
        } else {
            ++it; // Move to the next explosion
        }
    }
}

void SceneMain::explosionRender() {
    for (auto explosion : explosionList) {
        SDL_Rect srcRect = { explosion->currentFrame * explosion->width, 
                            0, 
                            explosion->width, 
                            explosion->height };
        SDL_Rect dstRect = { static_cast<int>(explosion->position.x),
                            static_cast<int>(explosion->position.y), 
                            explosion->width, 
                            explosion->height };
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &srcRect, &dstRect); // Render the explosion 
    }
}

void SceneMain::dropItem(Enemy *enemy) {
    // std::cout << "item droped!!!" << std::endl;
    float decide = dis(gen);
    if (decide < 0.3f) {
        initItemTemplate(itemType::Life);
    } else if (decide < 0.6f) {
        initItemTemplate(itemType::Shield);
    } else {
        initItemTemplate(itemType::Time);
    }

    Item* item = new Item(itemTemplate);
    item->position = enemy->position;
    float angle = dis(gen) * 2 * static_cast<float>(M_PI);
    item->direction = { static_cast<float>(cos(angle)), static_cast<float>(sin(angle)) };

    itemList.push_back(item);
}

void SceneMain::playerGetItem(Item *item) {
    if (isDead) {
        return;
    }
    score += 5;
    Mix_PlayChannel(-1, sounds["get_item"], 0);
    switch (item->type)
    {
    case itemType::Life:
        player.currentHealth += player.currentHealth < player.maxHealth ? 1 : 0; 
        break;
    default:
        break;
    }
}

void SceneMain::itemUpdate(float deltaTime) {
    for (auto it = itemList.begin(); it != itemList.end();) {
        auto item = *it;
        // update item position
        item->position.x += item->speed * item->direction.x * deltaTime;
        item->position.y += item->speed * item->direction.y * deltaTime;

        if ((item->position.x + item->width > game.getScreenWidth() || 
             item->position.x < 0) && 
             item->bounceCount > 0) {
            item->direction.x = -item->direction.x;
            item->bounceCount--;
        }
        if ((item->position.y + item->height > game.getScreenHeight() || 
             item->position.y < 0) &&
             item->bounceCount > 0) {
            item->direction.y = -item->direction.y;
            item->bounceCount--;
        }

        if (item->position.x + item->width < 0 ||
            item->position.x > game.getScreenWidth() ||
            item->position.y + item->height < 0 ||
            item->position.y > game.getScreenHeight()) {
            delete item;
            it = itemList.erase(it);
        } else {
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height
            };

            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };

            if (SDL_HasIntersection(&itemRect, &playerRect)) {
                playerGetItem(item);
                delete item;
                it = itemList.erase(it);
            } else {
                it++;
            }
        }
    }
}

void SceneMain::itemRender() {
    for (const auto& item : itemList) {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height 
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, nullptr, &itemRect);
    }
}

void SceneMain::initItemTemplate(itemType type) {
    const char* direct;
    switch (type)
    {
    case itemType::Life:
        direct = "assets/image/bonus_life.png";
        break;
    case itemType::Shield:
        direct = "assets/image/bonus_shield.png";
        break;
    case itemType::Time:
        direct = "assets/image/bonus_time.png";
        break;
    default:
        direct = nullptr;
        break;
    }
    if (direct == nullptr) {
        throw std::runtime_error("Invalid item type");
    }

    itemTemplate.texture = IMG_LoadTexture(game.getRenderer(), direct);
    SDL_QueryTexture(itemTemplate.texture, nullptr, nullptr, &itemTemplate.width, &itemTemplate.height);
    itemTemplate.width /= 4;
    itemTemplate.height /= 4;
}

void SceneMain::renderUI() {
    // Health
    int x = 10;
    int y = 10;
    int size = 32;
    int offset = 40;

    SDL_SetTextureColorMod(UIHealth, 100, 100, 100);
    for (int i = 0; i < player.maxHealth; i++) {
        SDL_Rect rect = {
            x + i * offset, y, size, size
        };
        SDL_RenderCopy(game.getRenderer(), UIHealth, nullptr, &rect);
    }
    SDL_SetTextureColorMod(UIHealth, 255, 255, 255);
    for (int i = 0; i < player.currentHealth; i++) {
        SDL_Rect rect = {
            x + i * offset, y, size, size
        };
        SDL_RenderCopy(game.getRenderer(), UIHealth, nullptr, &rect);
    }

    // Score
    auto text = "SCORE:" + std::to_string(score);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect rect = { game.getScreenWidth() - surface->w - 10, 10, surface->w, surface->h };
   
    SDL_RenderCopy(game.getRenderer(), texture, nullptr, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void SceneMain::changeSceneDelayed(float deltaTime, float delay) {
    timerEnd += deltaTime;
    if (timerEnd > delay) {
        auto sceneEnd = new SceneEnd();
        game.changeScene(sceneEnd);
    }
}
