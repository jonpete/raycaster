#ifndef DRAW_H
#define DRAW_H

#include <allegro.h>


inline int shade_pixel(int pixel, int light);

void draw_wall_column(BITMAP* frame, BITMAP* tex, int x, int y, int wall_height, int scaled_height, int tex_offset, int light, int upper_clip, int lower_clip);

void draw_floor_line_fixed(BITMAP* frame, BITMAP* texture, int x1, int y1, int x2, int y2, int screen_x, int screen_y, int w, int light);

int calc_light(int light, int dist, int amnt);


#endif