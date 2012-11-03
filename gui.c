#include <malloc.h>
#include <string.h>
#include <stdint.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_gfxBlitFunc.h>

#include "gui.h"

struct SCHbutton
{
    unsigned char flags;
    int posx, posy;
    unsigned int width, height;
    unsigned int color; /* кот прыгнул на нетбук и оставил это здесь: ygt6bhyju */
    char *text;
    TTF_Font *font;
    SDL_Surface *glyph;
};

#define FLAGCUT(btn_ptr,flag) btn_ptr->flags = button->flags&(~flag)
#define FONT_H 10

struct SCHbutton *SCHbtnCreate(struct SCHbtnInfo btninfo)
{
    struct SCHbutton *button = malloc(sizeof(struct SCHbutton));
    memset(button, 0 ,sizeof(struct SCHbutton));

    /* Setting it up */
    button->posx = btninfo.x;
    button->posy = btninfo.y;
    button->width = btninfo.w;
    button->height = btninfo.h;

    button->flags = (~(BTNSTATE_CLICKED|BTNSTATE_HOVER))&btninfo.flags;
    button->color = btninfo.col;

    if(btninfo.text == NULL)
        FLAGCUT(button, BTNV_TEXT);
    else
    {
        button->text = (char*)malloc(strlen(btninfo.text));
        strcpy(button->text, btninfo.text);

        if(btninfo.fontFN == NULL || !TTF_WasInit())
            FLAGCUT(button, BTNV_TEXT);
        else
        {
            button->font = TTF_OpenFont(btninfo.fontFN, FONT_H);
            if(button->font == NULL)
                FLAGCUT(button, BTNV_TEXT);
        }
    }

    if(btninfo.glyphFN == NULL)
        FLAGCUT(button, BTNV_GLYPH);
    else
    {
        if(btninfo.surfptr == NULL)
        {
            button->glyph = IMG_Load(btninfo.glyphFN);
            if(button->glyph == NULL)
                FLAGCUT(button, BTNV_GLYPH);
        }
        else
            button->glyph = btninfo.surfptr;
    }

    return button;
}

#define PTRCHK(ptr) if(ptr == NULL) return

void SCHbtnDraw(struct SCHbutton *who, SDL_Surface *where)
{
    PTRCHK(who); PTRCHK(where);

    /* Checking if there`s a "body visible" flag */
    if(who->flags&BTNV_BODY)
    {

        /* Background gradient */
//#define GRAD_STEP 1
        unsigned int col = who->color;
        unsigned int i;
        //int tick = 0;

        for(i = 0; i < who->height; i++)
        {
            lineColor(where, who->posx, who->posy+i, who->posx+who->width,
                who->posy+i, col);
            if((col&0xff) > 2)
                col -= 2;
        }

        /* Stroke */
        rectangleColor(where, who->posx, who->posy, who->posx+who->width,
                      who->posy+who->height, who->color|0xf0f0fff);

    }

    /* Glyph */
    if(who->flags&BTNV_GLYPH)
    {

        SDL_Rect rect;
        rect.x = who->posx+(who->width/2-who->glyph->w/2);
        rect.y = who->posy+(who->height/2-who->glyph->h/2);

        SDL_BlitSurface(who->glyph, NULL, where, &rect);

    }

    /* Text */
    if(who->flags&BTNV_TEXT && TTF_WasInit())
    {

        SDL_Color fontcolor = { (*(Uint8*)&who->color)|0x80,
                    (*(Uint8*)((intptr_t)&who->color+1))|0x80, (*(Uint8*)((intptr_t)&who->color+2))|0x80 };

        SDL_Surface *tmp = TTF_RenderText_Blended(who->font, who->text, fontcolor);

        SDL_Rect rect;
        rect.x = who->posx+(who->width/2-tmp->w/2);
        rect.y = who->posy+(who->height/2-tmp->h/2);

        SDL_BlitSurface(tmp, NULL, where, &rect);
        SDL_FreeSurface(tmp);

    }

    if(who->flags&BTNSTATE_HOVER)
    {
        rectangleColor(where, who->posx+1, who->posy+1, who->posx+who->width-1,
                      who->posy+who->height-1, who->color|0xf0f0fff);
    }

    if(who->flags&BTNSTATE_CLICKED)
    {
        boxColor(where, who->posx+1, who->posy+1, who->posx+who->width-1,
                      who->posy+who->height-1, who->color|0x4040440);
    }
}

int isrect(int x, int y, struct SCHbutton *btn)
{
    if
    (
        x > btn->posx && x < btn->posx+btn->width
        && y > btn->posy && y < btn->posy+btn->height
    ) return 1;
    return 0;
}

int buttons_hover = 0;
void SCHbtnLoop(struct SCHbutton *btn, SDL_Event *event)
{
    if(isrect(event->button.x, event->button.y, btn))
    {
        buttons_hover = 1;
        btn->flags |= BTNSTATE_HOVER;

        if(event->type == SDL_MOUSEBUTTONDOWN)
            btn->flags |= BTNSTATE_CLICKED;
        else
            btn->flags &= (~BTNSTATE_CLICKED);
    }
    else
    {
        btn->flags &= (~BTNSTATE_HOVER);

        if(buttons_hover)
            buttons_hover = 0;
    }
}

int SCHgetBtnState(struct SCHbutton *btn)
{
    return (BTNSTATE_CLICKED|BTNSTATE_HOVER)&btn->flags;
}

void SCHbtnToggleVisibility(struct SCHbutton *btn, char what)
{
    // not implemented yet!
}

int SCHisAnyBtnHover()
{
    return buttons_hover;
}
