#include <algorithm>

#include "viewport.h"
#include "draw.h"
#include "mathfunc.h"
#include "globals.h"

void Viewport:: draw_object(BITMAP *frame, Object *obj)		// Returns how much of the object it drew
{	
	const int light = (obj->light == FULL_BRIGHT ? 31 : obj->light);	// 31 is max light level
	BITMAP *sprite = sprites[obj->sprite];
			
	int distance = obj->distance;
		
	int dest_x = get_screen_x(obj->angle_to_view, frame->w);	
	int dest_w = (obj->w * projection) / distance;

	distance *= get_view_correction(dest_x, screen_w);
	int dest_y = get_screen_y(height, obj->height, distance, projection, screen_h);			
	int dest_h = (obj->h * projection) / distance;
		
	// Figure out direction to draw. Doom style formula (sort of)
	int dir_offset = 0;
	if(obj->has_directions)
	{
		float dir_angle = obj->angle;
		if(dir_angle < 0) dir_angle += TWO_PI;
		if(dir_angle >= TWO_PI) dir_angle -= TWO_PI;		

		dir_angle += atan2(obj->x - x, obj->y - y) + (PI/1.5);	// Not perfect, but close		
		dir_offset = int(dir_angle * (8.f / TWO_PI)) & 7;	// Eight direction sprites
	}
	int source_x = (dir_offset * obj->w) << 16;

	// 3 frames per animation state		
	int anim_state_offset;
	int start_source_y = 0;
	if(obj->is_animated)
	{
		anim_state_offset = (obj->anim * obj->h * 4);		
		start_source_y = (anim_state_offset + ((obj->frame / 15000) * obj->h)) << 16;
	}

	int source_y = start_source_y;
	const int delta_x = (obj->w << 16) / dest_w;		// Fixed point
	const int delta_y = (obj->h << 16) / dest_h;

	dest_y -= dest_h;		// Get these ready to draw
	dest_x -= dest_w / 2;
	dest_w += dest_x;		// These are now drawing end x & y, not width & height
	dest_h += dest_y;
	if(dest_w >= screen_w) dest_w = screen_w - 1;
	if(dest_h >= screen_h) dest_h = screen_h - 1;		
	if(dest_x < 0)
	{
		source_x += delta_x * -dest_x;
		dest_x = 0;
	}
	if(dest_y < 0)
	{
		start_source_y = delta_y * -dest_y;
		dest_y = 0;
	}
				
	const int start_dest_y = dest_y;
	const int start_dest_h = dest_h;
	unsigned short source_pixel;
		
	while(dest_x < dest_w)
	{				
		source_y = start_source_y;
		dest_y = start_dest_y;
		dest_h = start_dest_h;
		while(dest_h > dest_y && depthmap[dest_h * screen_w + dest_x] < distance) dest_h--;  // find lower clip	
		while(dest_y <= dest_h && depthmap[dest_y * screen_w + dest_x] < distance)	// find upper clip
		{
			dest_y++; 
			source_y += delta_y;
		}			

		while(dest_y < dest_h)
		{
			source_pixel = ((short*)sprite->line[source_y >> 16])[source_x >> 16];
			if(source_pixel != MASK_COLOR_16)	((short*)frame->line[dest_y])[dest_x] = source_pixel;
			dest_y++;
			source_y += delta_y;
		}		
		dest_x++;
		source_x += delta_x;			
	}				
}

void Viewport::	draw_objects(BITMAP *frame, Object *obj_list)
{
	const int max_visible_objects = 256;		
	unsigned i = 0;
	unsigned num_obj = 0;
	
	//Reallocate draw array if needed
	if(Object::total_num_objs > obj_draw_list_size)
	{
		delete[] objs_to_draw;
		obj_draw_list_size = Object::total_num_objs;
		objs_to_draw = new Object*[obj_draw_list_size];
	} 
	// TODO: make it reallocate if there are way less objects than the draw array
	
	for(Object *obj = obj_list; obj; obj = obj->next)
	{		
		obj->angle_to_view = get_angle_between_points(x, y, angle, obj->x, obj->y);			
		if(obj->angle_to_view < -1.f || obj->angle_to_view > 1.f) continue;

		obj->distance = sqrt((obj->x - x) * (obj->x - x) + (obj->y - y) * (obj->y - y));
		if(obj->distance < 24 || (obj->type == PLAYER && obj->distance < 64)) continue;
		

		if(obj->light != FULL_BRIGHT)	
		{
			if(obj->type == ENEMY && obj->anim == A_ATTACK)
			{
				if(obj->light < 8) obj->light = 8;
				obj->light *= 2;
			}
			obj->light = 100;
		}	
		
		objs_to_draw[num_obj] = obj;
		num_obj++;
	}

	std::sort(objs_to_draw, objs_to_draw + (num_obj), Object::obj_ptr_comp);
		
	if(num_obj > max_visible_objects) i = num_obj - max_visible_objects;		
	else i = 0;
		
	for(; i < num_obj; i++)		draw_object(frame, objs_to_draw[i]);	
}

void Viewport::	draw_walls(BITMAP *frame, Map *map)	
{
	int distance;
	int bottom, top, upper_bottom, upper_top;				
	int screen_y, upper_screen_y;
	int light, tex, floor_height;
	const Tile *tile, *last_tile;
		
	clear_floors(floors, screen_h);
		
	for(int screen_x = 0; screen_x < screen_w; screen_x++)
	{			
		screen_y = screen_h - 1;
		upper_screen_y = 0;	
			
		rays[screen_x].init(x, y, angle + atan2((screen_x - (screen_w >> 1)), projection));
		tile = map->get_tile(x, y);			
			
		while(tile->height != OUT_OF_BOUNDS && screen_y > upper_screen_y)
		{
			last_tile = tile;
			tile = rays[screen_x].cast(map);
				
			distance = ROUND(get_dist(x, y, angle, rays[screen_x].x, rays[screen_x].y));
			if(distance == 0) continue;
			
			top = get_screen_y(height, tile->height, distance, projection, screen_h);
			bottom = get_screen_y(height, last_tile->height, distance, projection, screen_h);
			upper_bottom = get_screen_y(height, tile->ceiling_height, distance, projection, screen_h);
			upper_top = get_screen_y(height, last_tile->ceiling_height, distance, projection, screen_h);
				
			if(last_tile->height != OUT_OF_BOUNDS)		// Get floor data
			{
				light = last_tile->get_light(L_FLOOR);
				tex = last_tile->get_tex(T_FLOOR);
				floor_height = height - last_tile->height;

				if(bottom < upper_screen_y) bottom = upper_screen_y;
				while(screen_y > bottom)
				{
					add_to_floors(floors, screen_x, screen_y, floor_height, tex, light);
					screen_y--;		
				}

				light = last_tile->get_light(L_CEILING);
				tex = last_tile->get_tex(T_CEILING);
				floor_height = height - last_tile->ceiling_height;

				if(upper_top > screen_y) upper_top = screen_y;
				while(upper_screen_y <= upper_top)
				{						
					add_to_floors(floors, screen_x, upper_screen_y, floor_height, tex, light);
					upper_screen_y ++;
				}
			}

			light =100;
				
			if(tile->height > last_tile->height && tile->height != OUT_OF_BOUNDS)
			{
				// The scaled height arg here is weird, but makes the textures smoother. Fix someday!
				draw_wall_column(frame, textures[tile->get_tex(T_LOWER)], screen_x, top, 
								tile->height, bottom + ((last_tile->height * projection) / distance) - top,
								rays[screen_x].tex_offset,upper_screen_y, screen_y);							
			}		
				
			if(top < upper_screen_y) top = upper_screen_y;
			while(screen_y > top)
			{
				depthmap[screen_y * screen_w + screen_x] = distance;	
				screen_y--;
			}			

			if(tile->get_tex(T_UPPER) != SKY_TEX && tile->ceiling_height < last_tile->ceiling_height 
					&& upper_screen_y <= screen_y && tile->height != OUT_OF_BOUNDS)
			{
				draw_wall_column(frame, textures[tile->get_tex(T_UPPER)], screen_x, upper_top, 
								last_tile->ceiling_height - tile->ceiling_height, upper_bottom - upper_top,
								rays[screen_x].tex_offset, upper_screen_y,  screen_y);
			}	

			if(upper_bottom > screen_y) upper_bottom = screen_y;
			while(upper_screen_y <= upper_bottom)
			{						
				depthmap[upper_screen_y * screen_w + screen_x] = distance;	
				upper_screen_y++;
			}
		}
	}
}

void Viewport::	draw_floors(BITMAP* frame)
{
	int distance;
	long long temp_distance;
	int source_x, source_y, source_x2, source_y2;
	int delta_x, delta_y;
				
	const int fixed_x = x * 65536.f;
	const int fixed_y = y * 65536.f;
	const int fixed_left_cos = cos(angle - FOV * 0.5f) * 65536.f;
	const int fixed_left_sin = sin(angle - FOV * 0.5f) * 65536.f;
	const int corrected_proj = projection * int(65536.f / cos(FOV * 0.5f));

	int *depthmap_line = depthmap;
	Floorline *fl;

	for(int fy = 0; fy < screen_h; fy ++)
	{
		if(fy == screen_h >> 1) continue;
		distance = corrected_proj / (fy - (screen_h >> 1));
		fl = floors[fy].floorlines;

		for(int fx, i = 0; i < floors[fy].size; i++)
		{
			fx = fl->x;			
			temp_distance = (distance * fl->height);		
				
			if(fl->texture != SKY_TEX)
			{					
				source_x = fixed_x + ((temp_distance * fixed_left_cos) >> 16);
				source_y = fixed_y + ((temp_distance * fixed_left_sin) >> 16);
				source_x2 = fixed_x + ((temp_distance * -fixed_left_sin) >> 16);
				source_y2 = fixed_y + ((temp_distance * fixed_left_cos) >> 16);

				delta_x = (source_x2 - source_x) / screen_w;
				delta_y = (source_y2 - source_y) / screen_w;
									
				source_x += (fx * delta_x);
				source_y += (fx * delta_y);
				source_x2 = source_x + (fl->w * delta_x);
				source_y2 = source_y + (fl->w * delta_y);
				
				draw_floor_line_fixed(frame, textures[fl->texture], source_x, source_y, source_x2, source_y2, 
									fx, fy, fl->w); 					
			}

			temp_distance = (temp_distance >> 16) * cos(FOV * 0.5f);
			for(int end = fx + fl->w; fx < end; fx++)	depthmap_line[fx] = temp_distance;
			fl++;
		}

		depthmap_line += screen_w;
	}		
}


