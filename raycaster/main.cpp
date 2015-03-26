#include <cmath>
#include <allegro.h>
#include <winalleg.h>	// For GetTickCount(). Windows.h won't work with allegro

#include "globals.h"
#include "floors.h"
#include "mathfunk.h"
#include "map.h"
#include "ray.h"
#include "object.h"
#include "enemy.h"
#include "draw.h"
#include "viewport.h"
#include "guns.h"

BITMAP *textures[32];
BITMAP *sprites[32];


int main()
{
	allegro_init();
	install_keyboard();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024,768,0,0);
	BITMAP* frame = create_bitmap(SCREEN_W, SCREEN_H);
	BITMAP* sky = load_bitmap("sky.bmp", NULL);
	
	textures[0] = create_bitmap(TILE_SIZE, TILE_SIZE);
	clear(textures[0]);
	textures[1] = load_bitmap("grass.bmp", NULL);
	textures[2] = load_bitmap("wall.bmp", NULL);
	textures[3] = load_bitmap("doom.bmp", NULL);
	textures[4] = load_bitmap("kanzi64.bmp", NULL);
	sprites[0] = load_bitmap("doomguy.bmp", NULL);	
	sprites[1] = load_bitmap("laser.bmp", NULL);
	sprites[2] = load_bitmap("table.bmp", NULL);
	sprites[3] = load_bitmap("pistol.bmp", NULL);

	init_guns();

	int sky_offset = 0;

	Map map;
	map.load("testmap.txt");
			
	// Start the list with the player
	Object *obj_list = create_object(300, 300, 0, 0, 64, 64, 0.8, true, true, PLAYER);
	Object::total_num_objs ++;		// create_object() doesn't increase this, but it probably should (append() does that now)
	Object *player = obj_list;

	int stats[] = {200, G_LASER, 0, 0};
	Object *test_enemy = append_obj(create_object(400, 400, 0, 0, 64, 64, 0.8, true, true, ENEMY, stats), obj_list);
	
	append_obj(create_object(700, 300, 0, 2, sprites[2]->w, sprites[2]->h, 0, false, false, BLOCK), obj_list);
	append_obj(create_object(764, 300, 0, 2, sprites[2]->w, sprites[2]->h, 0, false, false, BLOCK), obj_list);
	append_obj(create_object(812, 300, 0, 2, sprites[2]->w, sprites[2]->h, 0, false, false, BLOCK), obj_list);

	Viewport view(map.p_start_x, map.p_start_y, frame->w, frame->h, map.p_start_h + 64, map.p_start_angle);
		
	int now_time = GetTickCount();
	int old_time = now_time;
	float frame_time = 1;

	int fps_timer = now_time;	
	int fps = 0;
	int display_fps = 0;

	int player_fire_delay = 0;
	int player_gun = G_LASER;
	bool vsyncon = false;
	while(!key[KEY_ESC])
	{	
		old_time = now_time;
		now_time = GetTickCount();
		frame_time = (now_time - old_time) / 16.666f;	
		if(now_time - fps_timer > 1000)
		{
			fps_timer = now_time;
			display_fps = fps;
			fps = 0;
		}
		else fps ++;
		
		if(player_fire_delay > 0) player_fire_delay -= int(frame_time * 1000.f);

		if(key[KEY_RIGHT])	player->angle += 0.1 * frame_time;
		if(key[KEY_LEFT])	player->angle -= 0.1 * frame_time;
		if(player->angle < 0) player->angle += TWO_PI;	
		else if(player->angle >= TWO_PI) player->angle -= TWO_PI;			
		if(key[KEY_UP])		
		{
			player->velocity_angle = player->angle;
			if(key[KEY_LSHIFT])
			{
				player->velocity += 2 * frame_time;
				if(player->velocity > 10 ) player->velocity -= 2 * frame_time;
			}
			else
			{
				player->velocity += 1 * frame_time;
				if(player->velocity > 5) player->velocity -= 1 * frame_time;
			}
		}
		if(key[KEY_DOWN])
		{
			player->velocity_angle = player->angle;
			if(key[KEY_LSHIFT])
			{
				player->velocity -= 2 * frame_time;
				if(player->velocity < -10 ) player->velocity += 2 * frame_time;
			}
			else
			{
				player->velocity -= 1 * frame_time;
				if(player->velocity < -5) player->velocity += 1 * frame_time;
			}
		}
		if(key[KEY_SPACE] && player->vertical_velocity == 0) player->vertical_velocity = 5;
		
		if(key[KEY_LCONTROL])
		{
			if(player_fire_delay <= 0) 
			{
				Object*proj;
				switch(player_gun)
				{
					case G_LASER:					
						proj = fire_projectile(player, obj_list, guns[player_gun]);

						// This cheats it over to the right a bit to line up with the gun graphic:
						proj->y += cos(player->angle) * 8;
						proj->x -= sin(player->angle) * 8;

						player_fire_delay = guns[player_gun].recharge;
						view.light_mod = 10000;
					break;						
				}
			}
		}

		if(!key[KEY_V])
		{
			view.x = player->x;
			view.y = player->y;
			view.height = player->height + 64;
			view.angle = player->angle;
		}
		if(key[KEY_E])
		{
			view.x = test_enemy->x;
			view.y = test_enemy->y;
			view.height = test_enemy->height + 64;
			view.angle = test_enemy->angle;
		}
		
		if(key[KEY_F1]) vsyncon = true;
		if(key[KEY_F2]) vsyncon = false;

		if(view.light_mod > view.default_light) view.light_mod -= 500.f * frame_time;
		if(view.light_mod < view.default_light) view.light_mod = view.default_light;
		if(key[KEY_F]) view.light_mod = 10000;

 		// Just a test, make this nicer!
		sky_offset = -view.angle * (SCREEN_W / FOV) ;
		while(sky_offset < 0) sky_offset += SCREEN_W;
		while(sky_offset >= SCREEN_W) sky_offset -= SCREEN_W;				
		blit(sky, frame, 0, 0, sky_offset, 0, sky->w, sky->h);
		blit(sky, frame, 0, 0, sky_offset - (sky->w - 1), 0, sky->w, sky->h);	
		
		update_objects(obj_list, &map, player, frame_time);
		view.draw_walls(frame, &map);
		view.draw_floors(frame);		
		view.draw_objects(frame, obj_list);
		
		// Just a test - this is crap!
		// Gun graphic
		masked_blit(sprites[3], frame, 0, 0, frame->w/2 - sprites[3]->w/3, frame->h - sprites[3]->h, sprites[3]->w, sprites[3]->h);

		textprintf_ex(frame, font, 0, 0, 65535, 0, "FPS: %i  A: %f  ENEMY: %i  FIRE: %i  OBJNUM: %i", display_fps, view.angle, obj_list->next->state, player_fire_delay, Object::total_num_objs);
		if(vsyncon) vsync();
		blit(frame,screen,0,0,0 ,0,SCREEN_W,SCREEN_H);		
	}

	while(obj_list)
	{
		Object *temp = obj_list->next;
		delete obj_list;
		obj_list = temp;
	}

	destroy_bitmap(frame);
	allegro_exit();
	return 0;
} 
END_OF_MAIN()	// Allegro needs this
