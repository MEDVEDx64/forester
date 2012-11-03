#include "gui/gui.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Surface *screen = SDL_SetVideoMode(800,500,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    struct SCHbtnInfo bti = {20,20,32,32,0x0055aa55,BTNV_BODY|BTNV_TEXT,
                                             "lol","droid.ttf",NULL};
    struct SCHbutton *button1 = SCHbtnCreate(bti);
    int is_running = 1;
    while(is_running)
    {
        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
                case SDL_MOUSEBUTTONDOWN: SCHbtnLoop(button1, ev.button.x, ev.button.y, 1); break;
                default: SDL_MOUSEBUTTONDOWN: SCHbtnLoop(button1, ev.button.x, ev.button.y, 0); break;
            }
        }
        boxColor(screen, 0,0,200,200,0xff);
        SCHbtnDraw(button1,screen);
        SDL_Flip(screen);
    }

    return 0;
}
