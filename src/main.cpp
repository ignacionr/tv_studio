#include "sdladapters.h"
#include "scenes/forest.h"
#include "scenes/ice.h"

int main()
{
    sdl::UsesSDL use_sdl;
    sdl::UsesFont use_font;
    ForestScene::run();
    // IceScene::run();
    return 0;
}