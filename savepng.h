#pragma once
#ifndef _SDL_SAVEPNG
#define _SDL_SAVEPNG
#include <SDL.h>
/*
* SDL_SavePNG -- libpng-based SDL_Surface writer.
*
@@ -30,3 +32,5 @@ extern int SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *rw, int freedst);
* Return new SDL_Surface with a format suitable for PNG output.
*/
extern SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src);

#endif