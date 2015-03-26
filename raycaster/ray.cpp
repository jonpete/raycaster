#include "ray.h"

Tile* Ray::cast(Map* map)
{
	float x1, y1, x2, y2;
	float delta1 = 0.f;
	float delta2 = 0.f;
	
	Tile* tile = map->get_tile(x, y);
	Tile* tile2 = tile;
	
	const float x_move = (cosine_angle > 0.f) ? TILE_SIZE : -0.001f;
	const float y_move = (sine_angle > 0.f) ? TILE_SIZE : -0.001f;
	const float y_ratio = (cosine_angle != 0.f) ? sine_angle / cosine_angle : OUT_OF_BOUNDS;
	const float x_ratio = (sine_angle != 0.f) ? cosine_angle / sine_angle : OUT_OF_BOUNDS;
	while(tile2->compare(tile))
	{
		x1 = x2 = x;
		y1 = y2 = y;
		
		// & 0xffffffc0 = (int)x/64 * 64, where 64 = tilesize
		x1 = ((int)x1 & 0xffffffc0) + x_move;	
		y1 += (x1 - x) * y_ratio;
		delta1 = (x1 - x) * (x1 - x) + (y1 - y) * (y1 - y);
		y2 = ((int)y2 & 0xffffffc0) + y_move;
		x2 += (y2 - y) * x_ratio;
		delta2 = (x2 - x) * (x2 - x) + (y2 - y) * (y2 - y);
		
		if(delta1 < delta2)
		{
			x = x1;
			y = y1;								
		}
		else
		{
			x = x2;
			y = y2;				
		}	
		
		tile2 = map->get_tile(x, y);
	}
	
	if(delta1 <= delta2) 
	{
		if(cosine_angle > 0.f) 	
		{				
			side = L_WEST;
			tex_offset = (int)y & (TILE_SIZE - 1);
		}
		else 
		{
			side = L_EAST;
			tex_offset = TILE_SIZE - (int)y & (TILE_SIZE - 1);				
		}
	}
	else
	{
		if(sine_angle < 0.f) 
		{
			side = L_SOUTH;
			tex_offset = (int)x & (TILE_SIZE - 1);
		}
		else 	
		{
			side = L_NORTH;
			tex_offset = TILE_SIZE - (int)x & (TILE_SIZE - 1);								
		}
	}
			
	return tile2;			
}


void Ray::init(float start_x,float start_y,float angle)
{
	x = start_x;
	y = start_y;
	sine_angle = sin(angle);
	cosine_angle = cos(angle);
	tex_offset = 0;
}