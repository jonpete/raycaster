#include "globals.h"
#include "object.h"
#include "enemy.h"


void kill_enemy(Object *enemy)
{
	enemy->state = S_DEAD;
	start_obj_anim(enemy, A_DEAD);
}


int enemy_los(Object *enemy, Object *target, Map *map)
{
	const float enemy_view = get_angle_between_points(enemy->x, enemy->y, enemy->angle, target->x, target->y);
	if(enemy_view < -1.f || enemy_view > 1.f)	return 0;

	const float angle = atan2(target->y - enemy->y, target->x - enemy->x);
	const int x_mov = int(cos(angle) * 10.f);	// Kind of inaccurate to use regular ints. Float/fixed point?
	const int y_mov = int(sin(angle) * 10.f);
	int x = (int)enemy->x;
	int y = (int)enemy->y;
		
	for(int i = 0; i < 400; i++) // limit on steps to check
	{
		// Tile is too tall to see over
		if(map->get_height(x >> TILE_SHIFT, y >> TILE_SHIFT) >= enemy->height + enemy->h)	return 0;

		if(x >> TILE_SHIFT == (int)target->x >> TILE_SHIFT && y >> TILE_SHIFT == (int)target->y >> TILE_SHIFT) return 1;
		x += x_mov;
		y += y_mov;
	}

	return 0;
}


void enemy_follow(Object *enemy, Object *target)
{
	enemy->angle = atan2(target->y - enemy->y, target->x - enemy->x);
			
	if(enemy->velocity_angle > enemy->angle) 
	{
		enemy->velocity_angle -= 2;
		if(enemy->velocity_angle < enemy->angle) enemy->velocity_angle = enemy->angle;
	}
	if(enemy->velocity_angle < enemy->angle) 
	{
		enemy->velocity_angle += 2;
		if(enemy->velocity_angle > enemy->angle) enemy->velocity_angle = enemy->angle;
	}
	
	if(enemy->velocity < 5) enemy->velocity ++;	
}


void enemy_ai(Object *enemy, Object *target, Object *obj_list, Map *map)
{
	int dist;
	switch(enemy->state)
	{
		case S_DEFAULT:
			if(enemy_los(enemy, target, map))	enemy->state = S_FOLLOW;			
		break;	

		case S_HURT:			
			start_obj_anim(enemy, A_HURT); 			
		break;

		case S_JUMP:
			enemy->vertical_velocity = 5;
			enemy->state = S_FOLLOW;
		break;

		case S_FOLLOW:
			dist = (int)get_dist(enemy->x, enemy->y, enemy->angle, target->x, target->y);

			// TO DO: make enemy_los return a different value if target can be seen but shots will be blocked
			// dist %  makes the enemy not shoot all the damn time
			if((dist < 1000 || dist % 20 == 0) && enemy_los(enemy, target, map))	
			{
				enemy->angle = atan2(target->y - enemy->y, target->x - enemy->x);
				enemy->state = S_ATTACK;
				break;
			}
			else 
			{
				start_obj_anim(enemy, A_DEFAULT);
				enemy_follow(enemy, target);
			}
						
		break;

		case S_ATTACK:
			if(enemy->attrib[E_FIRE_DELAY] <= 0)
			{		
				start_obj_anim(enemy, A_ATTACK);

				// Fire a projectile to test
				dist = (int)get_dist(enemy->x, enemy->y, enemy->angle, target->x, target->y);
				
				Object *proj = fire_projectile(enemy, obj_list, 50, 10);

				// Try to angle the shot up/down
				if(dist != 0 && enemy->height != target->height)	
				{
					proj->vertical_velocity = (target->height - enemy->height) * (proj->velocity / dist);						
				}

				enemy->attrib[E_FIRE_DELAY] = 5000;				
			}
			enemy->state = S_FOLLOW;
		break;
	}		
}


