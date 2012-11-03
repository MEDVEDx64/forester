#include "tiles.h"
#include "utils.h"
#include "global.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL_rotozoom.h>

tilerecord_t *tiles = NULL;

int TilesLoad(const char *fname)
{
    FILE *f;
    if((f = fopen(fname, "r")) == NULL)
    {
        printf("Can`t reach the file: %s\n", fname);
        return -1;
    }

    /* Skipping to second line */
    //fscanf(f, "\n");

    int recieved = 0;
    int current_tile = 0;
    while(!feof(f))
    {
#define RESERVED 0x400
        if(tiles == NULL) tiles = malloc(sizeof(tilerecord_t));
        else tiles = realloc(tiles, sizeof(tilerecord_t)*(current_tile+1));
        tiles[current_tile].alright = 1; /* will be set to 0, once any fail happens */
        /* Reserving memory */
        tiles[current_tile].filename = malloc(RESERVED);
        memset(tiles[current_tile].filename, '\0', RESERVED);
        tiles[current_tile].dummy = malloc(RESERVED);
        if(fscanf(f, "%u:%u:%s\n", &tiles[current_tile].id, &tiles[current_tile].type,
                  tiles[current_tile].filename) != 3)
                  {
                      printf("TilesLoad(): WARNING: It seems like the input \
file have something wrong aboard (comments, etc.). Proceeding may result in fault!\n");
                      tiles[current_tile].alright = 0;
                  }
        tiles[current_tile].filename = realloc(tiles[current_tile].filename, strlen(tiles[current_tile].filename));
        free(tiles[current_tile].dummy);
        char *fn = (char*)malloc(strlen(tiles[current_tile].filename)+strlen(TILEDIR EXTEN)+1);
        memset(fn, '\0', strlen(tiles[current_tile].filename)+strlen(TILEDIR EXTEN)+1);
        strcat(fn, TILEDIR);
        strcat(fn, tiles[current_tile].filename);
        strcat(fn, EXTEN);

        SDL_Surface *tmpsurf = SDL_LoadBMP(fn);
        if(tmpsurf == NULL)
        {
            printf("TilesLoad(): Image wasn`t loaded successfully: %s\n", fn);
            tiles[current_tile].alright = 0;
            current_tile++;
            free(fn);
            continue;
        }
        free(fn);

        int x,y;
        tiles[current_tile].tile = SDL_DisplayFormatAlpha(tmpsurf);
        SDL_FreeSurface(tmpsurf);
        for(x = 0; x < tiles[current_tile].tile->w; x++)
        {
            for(y = 0; y < tiles[current_tile].tile->h; y++)
            {
                if(!(getPixel32(tiles[current_tile].tile, x, y)&0xff))
                    putPixel32(tiles[current_tile].tile, x, y, 0);
            }
        }

        if(tiles[current_tile].tile->w > TILE_W || tiles[current_tile].tile->h > TILE_H)
        {
            SDL_Surface *tmp = tiles[current_tile].tile;
            tiles[current_tile].tile = zoomSurface(tmp, (double)TILE_W/tmp->w, (double)TILE_H/tmp->h, SMOOTHING_OFF);
            SDL_FreeSurface(tmp);
        }

        current_tile++;
        recieved++;
    }

    return recieved;
}
