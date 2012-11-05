#include <stdio.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>
#include "gui.h"
#include "input.h"
#include "tiles.h"
#include "global.h"
#include "workspace.h"

#define VERSION_STR "0.1103_indev"

enum
{
    BUTTON_QUIT,
    BUTTON_NEW,
    BUTTON_LOAD,
    BUTTON_SAVE,
    BUTTON_RESIZE,
    BUTTON_TOOL_ERASE
};

#define TOOLBUTTON 6
struct SCHbutton *buttons[160];
unsigned int buttons_total = 0;
static TTF_Font *ui_font = NULL;

#define _1ST_LINE_H     16

SDL_Surface *screen = NULL;

int init()
{
    printf("Starting Forester "VERSION_STR"...\n");
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("SDL_Init() fail.\n");
        return 1;
    }

    screen = SDL_SetVideoMode(SCRW, SCRH, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        printf("Couldn`t create screen.\n");
        return 1;
    }

    SDL_WM_SetCaption("Forester "VERSION_STR, NULL);

    stringColor(screen, 2, 2, "Starting...", 0xccddeeff);
    SDL_Flip(screen);

    if(TTF_Init())
        printf("Warning: couldn`t initialize TTF.\n");
    else
    {
        ui_font = TTF_OpenFont(FONT_FN, UI_FONT_SIZE);
        if(ui_font == NULL)
            printf("Warning: font file has failed to load.\n");
    }

    buttons_total = TOOLBUTTON;
    struct SCHbtnInfo btn_in = {
        0, 0, 50, _1ST_LINE_H, 0x4488cc66,
        BTNV_BODY|BTNV_TEXT, "New", FONT_FN, NULL, NULL
    };
    buttons[BUTTON_NEW] = SCHbtnCreate(btn_in);

    btn_in.text = "Load";
    btn_in.x = 51;
    btn_in.col = 0x5566cc66;
    buttons[BUTTON_LOAD] = SCHbtnCreate(btn_in);

    btn_in.text = "Save";
    btn_in.x = 102;
    btn_in.col = 0x6655cc66;
    buttons[BUTTON_SAVE] = SCHbtnCreate(btn_in);

    btn_in.text = "Resize";
    btn_in.x = 153;
    btn_in.col = 0x7766cc66;
    buttons[BUTTON_RESIZE] = SCHbtnCreate(btn_in);

    btn_in.text = "Quit";
    btn_in.x = SCRW-50;
    btn_in.col = 0x88110066;
    buttons[BUTTON_QUIT] = SCHbtnCreate(btn_in);

    btn_in.flags = BTNV_BODY|BTNV_GLYPH;
    btn_in.glyphFN = "/usr/share/icons/default.kde4/32x32/actions/edit-select.png";
    btn_in.x = 0;
    btn_in.y = 17;
    btn_in.w = 32;
    btn_in.h = 32;
    btn_in.col = 0x3344cc66;
    buttons[BUTTON_TOOL_ERASE] = SCHbtnCreate(btn_in);

    if(WSinit())
        return 1;
    int toolc = WSgetToolsCount();
    if(toolc == -1)
        return 1;

    /* Tool buttons */

#define TBTN_W 50
#define TBTN_H 35

    btn_in.col = 0x22773366;
    btn_in.w = TBTN_W;
    btn_in.h = TBTN_H;
    btn_in.y = 50;

    int id = 0;
    int offsetx = 0;
    for(id = 0; id < toolc; id++)
    {
        btn_in.surfptr = tiles[id].tile;
        btn_in.x = (id-offsetx)*TBTN_W+id;
        if(btn_in.x+btn_in.w > SCRW)
        {
            offsetx = id;
            btn_in.y += TBTN_H+1;
            btn_in.x = 0;
        }

        buttons[id+TOOLBUTTON] = SCHbtnCreate(btn_in);
        buttons_total++;
    }

    printf("Alright.\n");
    return 0;
}

void drawState()
{
    char dat[0x1000];
    int w_ = 0;
    int h_ = 0;
    WSgetWH(&w_, &h_);
    char *tmp = WSgetFileName();
    sprintf(dat, "FILE: %s; W: %u; H: %u",
            (tmp == NULL ? "none" : tmp), w_, h_);
    free(tmp);

    SDL_Color fcol;
    fcol.r = 0xaa;
    fcol.g = 0xaa;
    fcol.b = 0xbb;
    SDL_Surface *fs = TTF_RenderText_Blended(ui_font, dat, fcol);

    SDL_Rect r;
    r.x = SCRW/2-fs->w/2;
    r.y = 1;

    SDL_BlitSurface(fs, NULL, screen, &r);
    SDL_FreeSurface(fs);

    fs = TTF_RenderText_Blended(ui_font, "Use arrow keys to move the workspace | 'P' to hide buttons", fcol);
    r.x = 2;
    r.y = SCRH-UI_FONT_SIZE-4;
    SDL_BlitSurface(fs, NULL, screen, &r);
    SDL_FreeSurface(fs);
}

#define KST_NONE        0
#define KST_DOWN        1
#define KST_PRESSED     2
#define KST_UP          3

#define REDSCR_TIME     3

int main(int argc, char *argv[])
{
    if(init()) return 1;
    int is_running = 1;
    char keyst[0x200];
    memset(&keyst, 0, 0x200);
    int _drawRedScreen = 0;

    while(is_running)
    {
        if(_drawRedScreen) _drawRedScreen --;
        boxColor(screen, 0, 0, SCRW-1, SCRH-1, 0xff);
        WSdraw(screen);

        int i;
        for(i = 0; i < buttons_total; i++)
        {
            SCHbtnDraw(buttons[i],screen);
        }

        int s; for(s = 0; s < 0x200; s++)
        {
            if(keyst[s] == KST_DOWN)
                keyst[s] = KST_PRESSED;
            if(keyst[s] == KST_UP)
                keyst[s] = KST_NONE;
        }

        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            if(!INisActive())
            {
                for(i = 0; i < buttons_total; i++)
                    SCHbtnLoop(buttons[i], &ev);
                WSloop(&ev);
            }

            /* Kbd handling */
            switch(ev.type)
            {
                case SDL_KEYDOWN: keyst[ev.key.keysym.sym] = KST_DOWN; break;
                case SDL_KEYUP: keyst[ev.key.keysym.sym] = KST_UP; break;
            }
        }

        if(INisActive())
        {

            int s,v;
            for(s = 0; s < 0x200; s++)
            {
                if(keyst[s] == KST_DOWN)
                {
                    if(!INsendKey(s))
                    {
                        for(v = 0; v < TOOLBUTTON; v++)
                        {
                            if(SCHgetBtnState(buttons[v])&BTNSTATE_CLICKED)
                            {
                                char *t = INgetResult();

                                if(v == BUTTON_LOAD)
                                {
                                    if(WSfread(t))
                                        _drawRedScreen = REDSCR_TIME;
                                }

                                else if(v == BUTTON_SAVE)
                                {
                                    if(WSfwrite(t))
                                        _drawRedScreen = REDSCR_TIME;
                                }

                                else if(v == BUTTON_RESIZE)
                                {
                                    unsigned int nw, nh;
                                    if(sscanf(t, "%u %u", &nw, &nh) == 2)
                                        WSresize(nw, nh);
                                    else
                                        _drawRedScreen = REDSCR_TIME;
                                }

                                free(t);
                                SCHbtnLoop(buttons[v], NULL);
                            }
                        }
                    }
                }
            }

        }
        else
        {

            /* Workspace movement */
            if(keyst[SDLK_UP])      WSmove(DIR_UP);
            if(keyst[SDLK_RIGHT])   WSmove(DIR_RIGHT);
            if(keyst[SDLK_DOWN])    WSmove(DIR_DOWN);
            if(keyst[SDLK_LEFT])    WSmove(DIR_LEFT);

            /* SCHbuttons proc */
            if(SCHgetBtnState(buttons[BUTTON_QUIT])&BTNSTATE_CLICKED)
                is_running = 0;

            if(SCHgetBtnState(buttons[BUTTON_NEW])&BTNSTATE_CLICKED)
                WSreset();

            if(SCHgetBtnState(buttons[BUTTON_LOAD])&BTNSTATE_CLICKED)
            {
                char *tmp = WSgetFileName();
                INstart("Load a file:", tmp);
                free(tmp);
            }

            if(SCHgetBtnState(buttons[BUTTON_SAVE])&BTNSTATE_CLICKED)
            {
                char *tmp = WSgetFileName();
                INstart("Save to file:", tmp);
                free(tmp);
            }

            int a;
            for(a = 0; a < WSgetToolsCount(); a++)
            {
                if(SCHgetBtnState(buttons[TOOLBUTTON+a])&BTNSTATE_CLICKED)
                    WStoolSet(a);
            }

        }

        drawState();
        if(INisActive())
            INdraw(screen, ui_font);

        if(_drawRedScreen)
            boxColor(screen, 0, 0, SCRW-1, SCRH-1, 0xcc0000aa);
        SDL_Flip(screen);
    }

    printf("Shuttin` down.\n");
    return 0;
}
