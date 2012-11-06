#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifdef DIST
    #define WORKDIR         "/usr/share/forester/data/"
#else
    #define WORKDIR         "data/"
#endif

#define ICN_ERASE       "/usr/share/icons/default.kde4/32x32/actions/edit-clear.png"
#define FONT_FN         "/usr/share/fonts/truetype/ttf-droid/DroidSansMono.ttf"
#define SCRW            960
#define SCRH            512
#define TILE_W          48
#define TILE_H          32
#define UI_FONT_SIZE    12

#endif // _GLOBAL_H
