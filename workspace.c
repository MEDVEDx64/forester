#include "workspace.h"
#include "global.h"
#include "tiles.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

char *cfname = NULL;

const char *keywds[] = {
    "name","width",
    "height", "array"
};

#define STR_NAME        0
#define STR_W           1
#define STR_H           2
#define STR_ARRAY       3

int tool = 0;

struct WSlocation
{
    char *name;
    unsigned int width, heigth;
    int *tiles;
}
location;

static int n_tiles = 0;

int WSinit()
{
    location.name = NULL;
    location.tiles = NULL;

    n_tiles = TilesLoad(ST_FILE);
    if(n_tiles == -1)
        return 1;

    return 0;
}

/* parser utils */

void parseLine(const char *str)
{
    int i;
    char *oldbuf = NULL;
    char *buf = (char*)malloc(strlen(str)+1);
    memset(buf, '\0', strlen(str)+1);
    strcpy(buf, str);
    for(i = 0; i < strlen(buf); i++)
    {
        if(buf[i] == ';' || buf[i] == '\n')
        {
            buf[i] = '\0';
            break;
        }
    }

    char *id_buf = (char*)malloc(strlen(buf)+1);
    strcpy(id_buf, buf);
    for(i = 0; i < strlen(id_buf); i++)
    {
        if(id_buf[i] == ':' || id_buf[i] == '=')
        {
            id_buf[i] = '\0';
            break;
        }
    }

    for(i = 0; i < /*sizeof(keywds)*/ 4; i++)
    {
        if(!strcasecmp(id_buf, keywds[i]))
        {
            oldbuf = buf;
            buf = (char*)((intptr_t)buf+strlen(id_buf)+1);
            break;
        }
    }

    switch(i)
    {
        case 0:
            if(location.name != NULL)
            {
                free(location.name);
                location.name = NULL;
            }
            location.name = (char*)malloc(strlen(buf));
            memset(location.name, '\0', strlen(buf));
            strcpy(location.name, buf); break;
        case 1: location.width = atoi(buf); break;
        case 2: location.heigth = atoi(buf); break;
        case 3:
            if(!location.width || !location.heigth) break;
            if(location.tiles != NULL)
            {
                free(location.tiles);
                location.tiles = NULL;
            }
            location.tiles = malloc((location.width*location.heigth)*sizeof(unsigned int));
            memset(location.tiles, 0, (location.width*location.heigth)*sizeof(unsigned int));
            char *dat = NULL; int d = 0;
            int found = 0;
            for(i = 0; i < strlen(buf)+1; i++)
            {
                if((buf[i] < '1' || buf[i] > '9') && buf[i] != '-')
                {
                    if(dat != NULL)
                    {
                        location.tiles[found] = atoi(dat);
                        free(dat);
                        dat = NULL;
                    }
                    d = 0;
                    found++;
                }
                else
                {
                    if(dat == NULL)
                    {
                        dat = (char*)malloc(strlen(buf));
                        memset(dat, '\0', strlen(buf));
                    }
                    dat[d] = buf[i];
                    d++;
                }
            }
            if(dat != NULL)
                free(dat);
            break;
    }
    free(oldbuf);
    free(id_buf);
}

int WSfread(const char *fn)
{
    FILE *f;
    if((f = fopen(fn, "r")) == NULL)
    {
        printf("Can`t reach the file: %s\n", fn);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long flen = ftell(f);
    rewind(f);

    void *data = malloc(flen+1);
    memset(data, '\0', flen+1);
    if(fread(data, flen, 1, f));
    fclose(f);

    parseLine((char*)data);
    int i;
    for(i = 0; i < flen; i++)
    {
        if(*(char*)((intptr_t)data+i) == '\n' && *(char*)((intptr_t)data+i) != '\0')
            parseLine((char*)((intptr_t)data+i+1));
    }

    cfname = (char*)realloc(cfname, strlen(fn));
    strcpy(cfname, fn);

    free(data);
    return 0;
}

int WSfwrite(const char *fn)
{
    if(location.name == NULL || location.tiles == NULL)
    {
        printf("WSfwrite(): incorrect location data.\n");
        return 1;
    }

    FILE *f;
    if((f = fopen(fn, "w")) == NULL)
    {
        printf("Can`t reach the file: %s\n", fn);
        return 1;
    }

    fprintf(f, "map\nname:%s\nwidth:%d\nheight:%d\narray=",
            location.name, location.width, location.heigth);
    int i;
    for(i = 0; i < (location.width*location.heigth); i++)
    {
        fprintf(f, "%d", location.tiles[i]);
        if(i == (location.width*location.heigth)-1)
            fprintf(f, "\n");
        else
            fprintf(f, " ");
    }

    cfname = (char*)realloc(cfname, strlen(fn));
    strcpy(cfname, fn);

    fclose(f);
    return 0;
}

int WSgetToolsCount()
{
    return n_tiles;
}

void WStoolSet(int tool_id)
{
    tool = tool_id;
}

static int ws_offsetx = 0;
static int ws_offsety = 0;

#define GRID_COLOR      0xaaff

void WSdraw(SDL_Surface *where)
{
    if(location.tiles == NULL) return;

    int x, y;
    for(x = 0; x < location.width; x++)
    {
        for(y = 0; y < location.heigth; y++)
        {
            SDL_Rect rect;
            if(!(y&1))
                rect.x = (x*TILE_W)+ws_offsetx;
            else
                rect.x = TILE_W/2+(x*TILE_W)+ws_offsetx;
            rect.y = (y*(TILE_H/2))+ws_offsety;
            if(location.tiles[y*location.width+x] < n_tiles && location.tiles[y*location.width+x] >= 0)
                SDL_BlitSurface(tiles[location.tiles[y*location.width+x]].tile,
                                NULL, where, &rect);
            else
            {
                SDL_BlitSurface(nil_tile, NULL, where, &rect);
            }
        }
    }
}

#define TILE_NIL -1

void WSresize(unsigned int w, unsigned int h)
{
    if(!w||!h) return;
    int *space = (int*)malloc((w*h)*sizeof(int));
    memset(space, TILE_NIL, (w*h)*sizeof(int));

    if(location.width && location.heigth && location.tiles != NULL)
    {
        int x,y;
        for(x = 0; x < location.width && x < w; x++)
        {
            for(y = 0; y < location.heigth && y < h; y++)
                space[y*w+x] = location.tiles[y*location.width+x];
        }
    }

    location.width = w;
    location.heigth = h;
    free(location.tiles);
    location.tiles = space;
}

#define CLCK_AREA 8
static int at_x = 0;
static int at_y = 0;

void WSloop(SDL_Event *event)
{
    int x,y;
    for(x = 0; x < location.width; x++)
    {
        for(y = 0; y < location.heigth; y++)
        {

#define CLCK_RECT_X1 ((!(y&1)) ? x*(TILE_W)+ws_offsetx+CLCK_AREA : x*(TILE_W)+ws_offsetx+CLCK_AREA+TILE_W/2)
#define CLCK_RECT_X2 ((!(y&1)) ? x*(TILE_W)+ws_offsetx+TILE_W-CLCK_AREA : x*(TILE_W)+ws_offsetx+TILE_W-CLCK_AREA+TILE_W/2)
#define CLCK_RECT_Y1 y*(TILE_H/2)+ws_offsety+CLCK_AREA
#define CLCK_RECT_Y2 y*(TILE_H/2)+ws_offsety+TILE_H-CLCK_AREA

            if(event->button.x > CLCK_RECT_X1 && event->button.x < CLCK_RECT_X2 &&
               event->button.y > CLCK_RECT_Y1 && event->button.y < CLCK_RECT_Y2)
               {
                   if(event->button.button == SDL_BUTTON_LEFT)
                        location.tiles[y*location.width+x] = tool;
                    at_x = x;
                    at_y = y;
                    break;
               }

        }
    }
}

#define WS_MOVEMENT_SPEED 4

void WSmove(char dir)
{
    if(dir&DIR_UP)      ws_offsety -= WS_MOVEMENT_SPEED;
    if(dir&DIR_RIGHT)   ws_offsetx += WS_MOVEMENT_SPEED;
    if(dir&DIR_DOWN)    ws_offsety += WS_MOVEMENT_SPEED;
    if(dir&DIR_LEFT)    ws_offsetx -= WS_MOVEMENT_SPEED;
}

char *WSgetFileName()
{
    if(cfname == NULL)
        return NULL;
    return strdup(cfname);
}

void WSgetWH(int *destw, int *desth)
{
    *destw = location.width;
    *desth = location.heigth;
}

#define DEF_NAME    "location"

void WSreset()
{
    if(location.name != NULL)
    {
        free(location.name);
        location.name = NULL;
    }

    location.name = (char*)malloc(strlen(DEF_NAME));
    strcpy(location.name, DEF_NAME);

    if(location.tiles != NULL)
    {
        free(location.tiles);
        location.tiles = NULL;
    }

    location.width = 0;
    location.heigth = 0;
    free(cfname);
    cfname = NULL;
}

void WSgetMousePos(int *destx, int *desty)
{
    *destx = at_x;
    *desty = at_y;
}
