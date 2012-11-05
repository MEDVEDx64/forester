#ifndef _INPUT_H
#define _INPUT_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

void INstart(const char *title, const char *initial);
/* returns 0 when recieves SDLK_RETURN, 1 when typing`s not done yet,
    -1 on failure */
int  INsendKey(char key);
void INdraw(SDL_Surface *where, TTF_Font *font);
int  INisActive();
char *INgetResult();

#endif // input.h
