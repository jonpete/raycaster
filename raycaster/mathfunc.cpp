#include "mathfunc.h"


float get_dist(float x, float y , float a, float x2, float y2)
{
	x = (x2 - x) * cos(a);
	y = -(y2 - y) * sin(a);
	return ((x > y) ? x - y : y - x);
}


float get_view_correction(int screen_x, int screen_w)		// Use with pythag distances
{
	return cos((screen_x - (screen_w >> 1)) * (FOV / screen_w));
}


float get_angle_between_points(float x1, float y1, float a1, float x2, float y2)
{
	float angle = atan2(y2 - y1, x2 - x1) - a1;
	if(angle < -PI) angle += TWO_PI;		// This range is important for get_screen_x()	
	return angle;
}


int get_screen_x(float angle_between, int screen_w)	
{
	// Angle must be between -PI and PI!
	return int((screen_w >> 1) + (screen_w >> 1) * tan(angle_between));
}


int get_screen_y(int view_height, int obj_height, int distance, int projection, int screen_h)
{
	return (screen_h >> 1) + (projection * (view_height - obj_height)) / (distance != 0 ? distance : 1);
}
