#include "sdladapters.h"
#include "scenes/forest.h"
#include "scenes/ice.h"

int main(int argc, char* argv[])
{
    sdl::UsesSDL use_sdl;
    sdl::UsesFont use_font;
    bool show_camera_pos = (argc > 1);
    ForestScene::run(show_camera_pos);
    // IceScene::run(show_camera_pos);
    return 0;
}