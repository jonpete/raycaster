#ifndef GLOBALS_H
#define GLOBALS_H

#include <allegro.h>
#include <cmath>

#define TILE_SIZE 64
#define TILE_SHIFT 6
#define SKY_TEX 0
#define FULL_BRIGHT 100		// Things that are always drawn at max light
#define OUT_OF_BOUNDS 999999
#define PI 3.14159f
#define TWO_PI 6.28318f
#define FOV 1.570795f	// FOV locked to 90 degrees
#define HALF_FOV 0.785398f

enum {T_FLOOR = 0, T_CEILING = 8, T_LOWER = 16, T_UPPER = 24};

extern BITMAP *textures[32];
extern BITMAP *sprites[32];

float get_dist(float x, float y, float a, float x2, float y2);

float get_angle_between_points(float x1, float y1, float a1, float x2, float y2);

int get_screen_x(float angle_between, int screen_w);

int get_screen_y(int view_height, int obj_height, float distance, int projection, int screen_h);


#endif