/*

    Forester Editor GUI system.
    MEDVEDx64, 2012.10.27.

*/

#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <SDL/SDL.h>

/* Button visibility flags */
#define BTNV_BODY           1
#define BTNV_TEXT           2
#define BTNV_GLYPH          4

/* Button state flags */
#define BTNSTATE_HOVER      8
#define BTNSTATE_CLICKED    16

/* Button FX flags */
#define BTNFX_SHADOW        32
#define BTNFX_GLOW          64

/* Button struct */
struct SCHbutton;
struct SCHbtnInfo
{
    int x, y;
    unsigned int w, h, col;
    unsigned char flags;
    char *text, *fontFN, *glyphFN;
    SDL_Surface *surfptr;
};

#ifdef __cplusplus
extern "C" {
#endif

struct SCHbutton    *SCHbtnCreate(struct SCHbtnInfo btninfo);
void                SCHbtnDraw(struct SCHbutton *who, SDL_Surface *where);
void                SCHbtnLoop(struct SCHbutton *btn, SDL_Event *event);
int                 SCHgetBtnState(struct SCHbutton *btn);

#define                                                                     \
SCHBTNFREI(btn_ptr)                                                         \
    do                                                                      \
    {                                                                       \
        if(btn_ptr != NULL)                                                 \
        {                                                                   \
            if(btn_ptr->text != NULL) free(btn_ptr->text);                  \
            if(btn_ptr->font != NULL) TTF_CloseFont(btn_ptr->font);         \
            if(btn_ptr->glyph != NULL) SDL_FreeSurface(btn_ptr->glyph);     \
            free(btn_ptr);                                                  \
            btn_ptr = NULL;                                                 \
        }                                                                   \
    }                                                                       \
    while(0)

#ifdef __cplusplus
}
#endif

#endif // GUI_H_INCLUDED
