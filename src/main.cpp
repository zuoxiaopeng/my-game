#include "Game.h"
#include "object.h"
#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>


int main(int , char**)
{
    Game& game = Game::gameInstance();
    game.initialize();
    game.run();
    return 0;
}

