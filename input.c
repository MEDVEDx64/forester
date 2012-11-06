#include "global.h"
#include "input.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include <SDL/SDL_gfxPrimitives.h>

#define IN_MAXL 0x1000
int in_mode = 0;
char *in_str = NULL;
char *in_title = NULL;
unsigned int in_cur = 0;

void INstart(const char *title, const char *initial)
{
    if(in_mode)
    {
        printf("warning: calling INstart() when input window has been already started.\n");
        return;
    }
    in_mode = 1;

    if(in_title != NULL)
    {
        free(in_title);
        in_title = NULL;
    }
    in_title = (char*)malloc(IN_MAXL);
    memset(in_title, '\0', IN_MAXL);

    in_cur = 0;
    if(in_str != NULL)
    {
        free(in_str);
        in_str = NULL;
    }
    in_str = (char*)malloc(IN_MAXL);
    memset(in_str, '\0', IN_MAXL);

    if(initial != NULL)
    {
        strcpy(in_str, initial);
        in_cur = strlen(initial);
    }

    if(title != NULL)
        strcpy(in_title, title);
}

int INsendKey(char key)
{
    if(!in_mode)
    {
        printf("warning: INsendKey() will not do it's work `til INstart() is called!\n");
        return -1;
    }

    if(key >= ' ' && key <= '~' && in_cur < IN_MAXL-1)
    {
        in_str[in_cur] = key;
        in_cur++;
    }

    else if(key == SDLK_BACKSPACE)
    {
        if(in_cur)
        {
            in_cur--;
            in_str[in_cur] = '\0';
        }
    }

    else if(key == SDLK_RETURN)
    {
        in_mode = 0;
        return 0;
    }

    return 1;
}

int INisActive()
{
    return in_mode;
}

char *INgetResult()
{
    return strdup(in_str);
}

#define IN_BGCOL                0x88
#define IN_BGCOL_ENTRY          0x8888
#define IN_TITLE_FONTC_R        0xaa
#define IN_TITLE_FONTC_G        0xaa
#define IN_TITLE_FONTC_B        0xaa
#define IN_ENTRY_FONTC_R        0xaa
#define IN_ENTRY_FONTC_G        0xaa
#define IN_ENTRY_FONTC_B        0x11

void INdraw(SDL_Surface *where, TTF_Font *font)
{
    boxColor(where, 0, 0, SCRW-1, SCRH-1, IN_BGCOL);
    SDL_Color tcol;
    tcol.r = IN_TITLE_FONTC_R;
    tcol.g = IN_TITLE_FONTC_G;
    tcol.b = IN_TITLE_FONTC_B;
    SDL_Color tcol_entry;
    tcol_entry.r = IN_ENTRY_FONTC_R;
    tcol_entry.g = IN_ENTRY_FONTC_G;
    tcol_entry.b = IN_ENTRY_FONTC_B;

    boxColor(where, 0, SCRH/2, SCRW-1, SCRH/2+UI_FONT_SIZE, IN_BGCOL_ENTRY);

    SDL_Surface *tx = NULL;
    SDL_Rect r;

    if(in_title != NULL)
    {
        tx = TTF_RenderText_Blended(font, in_title, tcol);
        if(tx != NULL)
        {
            r.x = SCRW/2-tx->w/2;
            r.y = SCRH/2-tx->h-1;
            SDL_BlitSurface(tx, NULL, where, &r);
            SDL_FreeSurface(tx);
        }
    }

    if(in_str != NULL)
    {
        tx = TTF_RenderText_Blended(font, in_str, tcol_entry);
        if(tx != NULL)
        {
            r.x = SCRW/2-tx->w/2;
            r.y = SCRH/2-1;
            SDL_BlitSurface(tx, NULL, where, &r);
            SDL_FreeSurface(tx);
        }
    }
}
