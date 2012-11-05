#ifndef _WORKSPACE_H
#define _WORKSPACE_H

#include <SDL/SDL.h>

/* Direction flags */
#define DIR_UP          1
#define DIR_RIGHT       2
#define DIR_DOWN        4
#define DIR_LEFT        8

void    WSdraw(SDL_Surface *where);
int     WSinit();
int     WSfread(const char *fn);
int     WSfwrite(const char *fn);
void    WSloop(SDL_Event *event);
void    WStoolSet(int tool_id);
int     WSgetToolsCount();
void    WSresize(unsigned int w, unsigned int h);
char    *WSgetFileName();
void    WSmove(char dir);
void    WSgetWH(int *destw, int *desth);
void    WSreset();

#endif /* workspace.h */
