#ifndef GLOBALS_H
#define GLOBALS_H

#include <allegro.h>

#define TILE_SIZE 64
#define TILE_SHIFT 6
#define SKY_TEX 0
#define FULL_BRIGHT 100		// Things that are always drawn at max light
#define OUT_OF_BOUNDS 999999

// Light & texture bitshifts. 5 bits for each light side, 8 bits for texture
enum {L_FLOOR = 0, L_CEILING = 5, L_NORTH = 10, L_SOUTH = 15, L_EAST = 20, L_WEST = 25};
enum {T_FLOOR = 0, T_CEILING = 8, T_LOWER = 16, T_UPPER = 24};

extern BITMAP *textures[32];
extern BITMAP *sprites[32];


#endif