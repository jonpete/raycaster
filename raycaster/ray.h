#ifndef RAY_H
#define RAY_H

#include "map.h"


class Ray	
{
	public:
	float x, y, sine_angle, cosine_angle;
	int tex_offset, side;

	Tile* cast(Map* map);
	
	void init(float start_x,float start_y,float angle);
};



#endif