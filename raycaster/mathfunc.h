#ifndef MATHFUNC_H
#define MATHFUNC_H

#include <cmath>

#define PI 3.14159f
#define TWO_PI 6.28318f
#define FOV 1.570795f	// FOV locked to 90 degrees
#define ROUND(n) ((n > 0) ? floorf((n) + 0.5f) : ceilf((n) - 0.5f))


float get_dist(float x, float y , float a, float x2, float y2);

float get_view_correction(int screen_x, int screen_w);

float get_angle_between_points(float x1, float y1, float a1, float x2, float y2);

int get_screen_x(float angle_between, int screen_w);

int get_screen_y(int view_height, int obj_height, int distance, int projection, int screen_h);

#endif