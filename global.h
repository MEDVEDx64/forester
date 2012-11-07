#ifndef _GLOBAL_H
#define _GLOBAL_H

#ifdef DIST
    #define BASE_DIR         "/usr/share/forester/"
#else
    #define BASE_DIR         "./"
#endif
#define DATA_DIR         BASE_DIR"data/"

extern unsigned short scr_w_;
extern unsigned short scr_h_;

#define ICN_ERASE       DATA_DIR"edit-clear.png"
#define FONT_FN         DATA_DIR"DroidSansMono.ttf"
#define CFG_FN          BASE_DIR"config"
#define SCRW_D          1024
#define SCRH_D          680
#define TILE_W          48
#define TILE_H          32
#define UI_FONT_SIZE    12

#endif // _GLOBAL_H
