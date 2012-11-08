#include "utils.h"

Uint32 getPixel32(SDL_Surface *surface, int x, int y) /* lazyfoo */
{
    if(x<0 || y<0)
        return 0;
    SDL_LockSurface(surface);
    Uint32 *pixels = (Uint32*)surface->pixels;
    SDL_UnlockSurface(surface);
    return pixels[(y*surface->w)+x];
}

void putPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel) /* lazyfoo */
{
    if(x<0 || y<0)
        return;
    SDL_LockSurface(surface);
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[ ( y * surface->w ) + x ] = pixel;
    SDL_UnlockSurface(surface);
}
