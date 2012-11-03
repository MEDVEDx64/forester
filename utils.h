#ifndef _UTILS_H
#define _UTILS_H

#include <SDL/SDL.h>

Uint32 getPixel32(SDL_Surface *surface, int x, int y); /* lazyfoo */
void putPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel); /* lazyfoo */

#endif
