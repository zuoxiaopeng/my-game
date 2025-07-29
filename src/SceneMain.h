#ifndef SCENEMAIN_H
#define SCENEMAIN_H
#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <SDL_mixer.h>
#include <map>
#include <SDL_ttf.h>

class Game; // Forward declaration of Game class

class SceneMain : public Scene {
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void cleanup() override;

    void keybordControl(float deltaTime); // Keyboard control function

    // Player functions
    void playerUpdate(float deltaTime); // Player update function
    void playerExplode(SDL_FPoint position); // Player explosion function
    
    // Player shooting function
    void palayerShoot(); // Shooting function
    void projectileUpdate(float deltaTime); // Projectile update function
    void projectileRender(); // Projectile render function


    // Enemy functions
    void enemyUpdate(float deltaTime); // Enemy update function
    void enemyRender(); // Enemy render function
    void spawnEnemy(); // Enemy spawn function

    void enemyShoot(Enemy* enemy); // Enemy shooting function
    void enemyProjectileUpdate(float deltaTime); // Enemy projectile update function
    void enemyProjectileRender(); // Enemy projectile render function
    
    SDL_FPoint getDirection(Enemy* enemy); // Get direction function
    void enemyExplode(Enemy* enemy); // Explosion function

    // Explosion function
    void explosionUpdate(float deltaTime); // Explosion update function
    void explosionRender(); // Explosion render function

    // item
    void dropItem(Enemy* enemy);
    void playerGetItem(Item* item);
    void itemUpdate(float);
    void itemRender();
    void initItemTemplate(itemType type); 

    // UI
    void renderUI();

    // end delay
    void changeSceneDelayed(float delatTime, float delay);


private:
    Player player; // Player object

    // Random number generation
    std::mt19937 gen; // Random number generator engine
    std::uniform_real_distribution<float> dis;

    // player
    bool isDead = false; // Player death flag
    projectilePlayer projectilePlayerTemplate; // Projectile template object
    std::list<projectilePlayer*> projectilePlayerList; // List of projectiles

    // Enemy
    Enemy enemyTemplate; // Enemy object
    std::list<Enemy*> enemyList; // List of enemies

    projectileEnemy projectileEnemyTemplate; // Enemy projectile template object
    std::list<projectileEnemy*> projectileEnemyList; // List of enemy projectiles

    // Explosion
    Explosion explosionTemplate; // Explosion object
    std::list<Explosion*> explosionList; // List of explosions

    // Item
    Item itemTemplate;
    std::list<Item*> itemList;

    // music
    Mix_Music* bgm;
    std::map<std::string, Mix_Chunk*> sounds; 

    // UI
    SDL_Texture* UIHealth;
    TTF_Font* scoreFont;
    int score = 0;

    // end time delay
    float timerEnd = 0.0f;
    
};

#endif // SCENEMAIN_H