#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

// player object
struct Player {
    SDL_Texture* texture = nullptr; // Texture for the player
    SDL_FPoint position = {0, 0}; // Position of the player
    int width = 0; // Width of the player
    int height = 0; // Height of the player
    int speed = 500; // Speed of the player
    int currentHealth = 3; // Current health of the player
    int maxHealth = 3;
    Uint32 cooldown = 300; // Cooldown time for shooting
    Uint32 lastShootTime = 0; // Last time the player shot
};

struct projectilePlayer {
    SDL_Texture* texture = nullptr; // Texture for the projectile
    SDL_FPoint position = {0, 0}; // Position of the projectile
    int width = 0; // Width of the projectile
    int height = 0; // Height of the projectile
    int speed = 1000; // Speed of the projectile
    int damage = 1; // Damage of the projectile
};

// enemy object
struct Enemy {
    SDL_Texture* texture = nullptr; // Texture for the enemy
    SDL_FPoint position = {0, 0}; // Position of the enemy
    int width = 0; // Width of the enemy
    int height = 0; // Height of the enemy
    int speed = 300; // Speed of the enemy
    int currentHealth = 2; // Current health of the enemy
    Uint32 cooldown = 3000; // Cooldown time for shooting
    Uint32 lastShootTime = 0; // Last time the projectile was shot
};

struct projectileEnemy {
    SDL_Texture* texture = nullptr; // Texture for the projectile
    SDL_FPoint position = {0, 0}; // Position of the projectile
    SDL_FPoint direction = {0, 0}; // Direction of the projectile
    int width = 0; // Width of the projectile
    int height = 0; // Height of the projectile
    int speed = 400; // Speed of the projectile
    int damage = 1; // Damage of the projectile
};

struct Explosion {
    SDL_Texture* texture = nullptr; // Texture for the explosion
    SDL_FPoint position = {0, 0}; // Position of the explosion
    int width = 0; // Width of the explosion
    int height = 0; // Height of the explosion
    int currentFrame = 0; // Current frame of the explosion
    int totalFrames = 0; // Total frames of the explosion
    Uint32 startTime = 0; // Last time the frame was updated
    Uint32 FPS = 10; // Frames per second of the explosion
};

// Item
enum itemType {
    Life,
    Shield,
    Time
};

struct Item {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int bounceCount = 3;
    itemType type = itemType::Life;
};

// Background 
struct Background {
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = { 0, 0 };
    float offset = 0;
    int width = 0;
    int height = 0;
    int speed = 20;
};

#endif // OBJECT_H