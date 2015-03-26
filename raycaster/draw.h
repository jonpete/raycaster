#ifndef DRAW_H
#define DRAW_H

#include <allegro.h>


void draw_wall_column(BITMAP* frame, BITMAP* tex, int x, int y, int wall_height, int scaled_height, int tex_offset, int upper_clip, int lower_clip);

void draw_floor_line_fixed(BITMAP* frame, BITMAP* texture, int x1, int y1, int x2, int y2, int screen_x, int screen_y, int w);



#endif