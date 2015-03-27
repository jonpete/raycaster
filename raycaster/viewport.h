#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "floors.h"
#include "ray.h"
#include "object.h"
#include <allegro.h>

class Viewport			
{
	public:	
	float x, y, angle;	
	int height;
	int light_mod;
	int default_light;
	int screen_w, screen_h, projection;
	int *depthmap;
	Ray *rays;
	Floormap *floors;

	Object **objs_to_draw;
	int obj_draw_list_size;

	void draw_object(BITMAP *frame, Object *obj);

	void draw_objects(BITMAP *frame, Object *obj_list);
	

	void draw_walls(BITMAP *frame, Map *map);

	void draw_floors(BITMAP* frame);

	Viewport(int sx, int sy, int start_sw, int start_sh, int start_height, float sa = 0)
	{
		x = sx;
		y = sy;
		screen_w = start_sw;
		screen_h = start_sh;
		angle = sa;
		height = start_height;
		projection = screen_w / 2;	// Creates 90 degree FOV
		default_light = 8000;
		light_mod = 8000;	// Zero is no distance fading
		rays = new Ray[screen_w];	
		for (int i = 0; i < screen_w; i++)
		{
			rays[i].angle_offset = atan2((i - (screen_w >> 1)), projection);
		}
		floors = new Floormap[screen_h];		
		depthmap = new int[screen_w * screen_h];
		std::fill(depthmap, depthmap + (screen_w * screen_h), OUT_OF_BOUNDS);		

		objs_to_draw = new Object*[Object::total_num_objs];
		obj_draw_list_size = Object::total_num_objs;
	}

	~Viewport()	{
		delete [] rays;		
		delete [] floors;
		delete [] depthmap;	}
};


#endif VIEWPORT_H