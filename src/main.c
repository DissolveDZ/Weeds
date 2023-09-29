#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cglm/call.h>
#include <cglm/struct.h>
#include "win_include.h"
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "container.h"
#include "def.h"
#include "init.c"
#include "update.c"
#include "inventory.c"
#include "draw.c"

int main()
{
    init();
    while (!state->quit)
    {
        update();
        draw();
    }
    EngineQuit();
    return 0;
}