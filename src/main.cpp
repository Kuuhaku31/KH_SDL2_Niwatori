
// run.cpp

#define SDL_MAIN_HANDLED

#include "game_manager.h"

int
main(int argc, char** argv)
{
    GameManager::Instance().Run(argc, argv);
    return 0;
}
