#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include <SDL/SDL.h>
#define ST_FILE DATA_DIR"statics"
#define TILEDIR DATA_DIR"tiles/"
#define EXTEN   ".bmp"

typedef struct
{
    char alright;
    unsigned int id, type;
    char *filename;
    void *dummy;
    SDL_Surface *tile;
}
tilerecord_t;

extern tilerecord_t *tiles;
extern SDL_Surface *nil_tile;

/* returns count of loaded tiles */
/* -1 on fail */
int TilesLoad(const char *fname);

#endif // FILES_H_INCLUDED
