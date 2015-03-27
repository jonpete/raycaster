#include "object.h"
#include "enemy.h"

// TODO:
// Change most uses of object->h to object->w - 
// the reason being that these are 2d sprites standing up in a 3d world,
// the "h" dimension should only be used for real up/down
// and the "w" used for anything happening in the 2d map (forward, back, left, right)

int Object::total_num_objs = 0;

Object* append_obj(Object *obj, Object *obj_list)
{
	while(obj_list->next) obj_list = obj_list->next;	
	obj_list->next = obj;
	obj->prev = obj_list;
	Object::total_num_objs++;
	return obj;
}


Object* remove_obj(Object *obj)
{
	Object *temp = 0;
	
	if(obj->prev && obj->next)
	{
		obj->prev->next = obj->next;
		obj->next->prev = obj->prev;
		temp = obj->prev;
		delete obj;		
	}
	else if(obj->prev && !obj->next)
	{
		obj->prev->next = obj->next;
		temp = obj->prev;
		delete obj;		
	}
	else if(!obj->prev && obj->next)
	{
		obj->next->prev = 0;
		temp = obj->next;
		delete obj;		
	}
	else delete obj;

	//if(Object::total_num_objs > 0)
	Object::total_num_objs--;
	return temp;
}


Object* create_object(float x, float y, float height, int sprite, int w, int h, float angle, bool has_directions, bool is_animated, obj_type type, int *attrib)
{
	Object *obj = new Object;
	obj->x = x;
	obj->y = y;
	obj->height = height;
	obj->sprite = sprite;
	obj->w = w;
	obj->h = h;
	obj->angle = angle;
	obj->velocity_angle = angle;
	obj->vertical_velocity = 0;
	obj->velocity = 0;
	obj->frame = 0;
	obj->anim = A_DEFAULT;
	obj->state = S_DEFAULT;
	obj->type = type;
	obj->is_animated = is_animated;
	obj->has_directions = has_directions;	
	obj->prev = 0;
	obj->next = 0;
	for(int i = 0; i < NUM_ATTRIB; i ++) obj->attrib[i] = (attrib ? attrib[i] : 0);	// Set to zero if no attrib array pointer supplied	
	return obj;
}


bool start_obj_anim(Object *obj, anim_state anim, bool safe)
{
	if(safe && obj->anim == anim) return false;
	obj->anim = anim;
	obj->frame = 0;
	return true;
}


bool obj_collide(Object *obj1, Object *obj2)
{
	if(obj1 == obj2) return false;	

	int min_dist = obj1->w / 2 + obj2->w / 2;
	if(obj1->type == PROJECTILE) 
	{
		min_dist = min_dist / 2;
		if(min_dist == 0) min_dist = 1;
	}
	// Returns true if objects are too close (collision)
	return min_dist > sqrt((obj2->y - obj1->y) * (obj2->y - obj1->y) + (obj2->x - obj1->x) * (obj2->x - obj1->x));
}


int test_obj_map_collisions(Object *obj, Map *map, int allowable_height)
{	
	int height;
	
	height = map->get_height(int(obj->x - obj->w/2) / TILE_SIZE, int(obj->y - obj->w/2) / TILE_SIZE); 
	if(height == OUT_OF_BOUNDS || height > obj->height + allowable_height) return height;
	
	height = map->get_height(int(obj->x + obj->w/2) / TILE_SIZE, int(obj->y - obj->w/2) / TILE_SIZE); 
	if(height == OUT_OF_BOUNDS || height > obj->height + allowable_height) return height;

	height = map->get_height(int(obj->x - obj->w/2) / TILE_SIZE, int(obj->y + obj->w/2) / TILE_SIZE); 
	if(height == OUT_OF_BOUNDS || height > obj->height + allowable_height) return height;

	height = map->get_height(int(obj->x + obj->w/2) / TILE_SIZE, int(obj->y + obj->w/2) / TILE_SIZE); 
	if(height == OUT_OF_BOUNDS || height > obj->height + allowable_height) return height;

	return 0;
}


Object* test_all_obj_collisions(Object *obj, Object *obj_list)
{	
	Object *test_obj = obj_list;		// should be a pointer to first object in the list
	for(; test_obj; test_obj = test_obj->next)
	{
		if(test_obj->type == PROJECTILE) break;
		if(test_obj->type == PASS || test_obj->state == S_DEAD) continue;		
		if(obj_collide(obj, test_obj)) 
		{
			if(obj->height > test_obj->height)
			{
				if(obj->height - test_obj->height < test_obj->h) 	return test_obj;
			}			
			else if(test_obj->height - obj->height < obj->h)	return test_obj;
		}
	}

	return 0;
}


int update_animation(Object *obj, float frame_time)
{
	obj->frame += int(2000.f * frame_time);			
	if(obj->frame >= 60000) 
	{
		if(obj->state != S_DEAD)
		{
			obj->anim = A_DEFAULT;		
			obj->frame -= 60000;
		}
		else obj->frame = 59999;		// This should make death animation freeze on last frame?
		return 1;
	}
	return 0;
}


int move_object(Object *obj, Object *obj_list, Map *map, float frame_time)
{
	const float gravity = 0.2f;
	const float friction = 0.1f;
	const int stair_height = 16; // Max height you can walk up without jumping
	float start_x, start_y;
	int map_height = 0;
	Object *collided = 0;
	
	start_x = obj->x;
	start_y = obj->y;

	int num = 1;	// Projectile num collision checks
	float mul = 1.f;

	switch (obj->type)
	{
		case PROJECTILE:	
					
			// If moving very fast (like a bullet), check the collisions a few times
			if(obj->velocity > TILE_SIZE * 4) num = 16;
			else if(obj->velocity > TILE_SIZE * 2) num = 8;
			else if(obj->velocity > TILE_SIZE) num = 4;
			else if(obj->velocity > TILE_SIZE / 2) num = 2;

			mul = 1.f / (float)num;
			for(int i = 0; i < num; i++)
			{
				obj->x += cos(obj->velocity_angle) * (obj->velocity * mul) * frame_time;
				obj->y += sin(obj->velocity_angle) * (obj->velocity * mul) * frame_time;
				obj->height += (obj->vertical_velocity * mul) * frame_time;

				collided = test_all_obj_collisions(obj, obj_list);
				map_height = test_obj_map_collisions(obj, map);
				if(map_height || collided) break;
				else
				{
					start_x = obj->x;
					start_y = obj->y;
				}
			}

			if((map_height || collided) && obj->state != S_ATTACK)
			{
				obj->state = S_ATTACK;		// Explosion
				obj->x = start_x;
				obj->y = start_y;
				obj->velocity = 0;
				obj->vertical_velocity = 0;
				start_obj_anim(obj, A_ATTACK);				
				
				if(collided)	
				{
					collided->state = S_HURT;
					if(collided->type == ENEMY)
					{
						collided->attrib[E_HEALTH] -= obj->attrib[P_DAMAGE];
						if(collided->attrib[E_HEALTH] <= 0) kill_enemy(collided);
					}
				}
			}
		break;

		case PLAYER:
		case ENEMY:
			if(obj->velocity != 0)
			{
				obj->x += cos(obj->velocity_angle) * obj->velocity * frame_time;
				collided = test_all_obj_collisions(obj, obj_list);
				map_height = test_obj_map_collisions(obj, map, stair_height);
				if(map_height)	
				{
					obj->x = start_x;
					if(obj->type == ENEMY && map_height - obj->height < obj->h + obj->h / 2) obj->state = S_JUMP;
				}
				else if(collided && collided->type != PROJECTILE)	obj->x = start_x;					
								
				obj->y += sin(obj->velocity_angle) * obj->velocity * frame_time;
				collided = test_all_obj_collisions(obj, obj_list);
				map_height = test_obj_map_collisions(obj, map, stair_height);
				if(map_height) 
				{
					obj->y = start_y;
					if(obj->type == ENEMY && map_height - obj->height < obj->h + obj->h / 2) obj->state = S_JUMP;
				}
				else if(collided && collided->type != PROJECTILE)	obj->y = start_y;
													
				if(start_x != obj->x || start_y != obj->y)
				{
					map_height = map->get_height((int)obj->x >> TILE_SHIFT, (int)obj->y >> TILE_SHIFT);
					// Move up a stair step height max
					if(map_height > obj->height && map_height < obj->height + stair_height) 
					{
						obj->height = (float)map_height;
						obj->vertical_velocity = 0;
					}						
				}
			}
					
			if(obj->velocity > 0) 
			{
				obj->velocity -= friction * frame_time;
				if(obj->velocity < 0) obj->velocity = 0;
			}
			else if(obj->velocity < 0) 
			{
				obj->velocity += friction * frame_time;
				if(obj->velocity > 0) obj->velocity = 0;
			}
					
			// Do vertical/gravity stuff			
			obj->height += obj->vertical_velocity * frame_time;
			map_height = test_obj_map_collisions(obj, map, obj->vertical_velocity >= 0 ? stair_height : 0);
			if(map_height) 
			{				
				obj->height = map_height;	
				obj->vertical_velocity = 0;
			}
			else if(test_all_obj_collisions(obj, obj_list))	
			{
				obj->height -= obj->vertical_velocity * frame_time;
				obj->vertical_velocity = 0;
			}
			else obj->vertical_velocity -= gravity * frame_time;		
			
			// Floor collision (could be merged with map collision up there in the future)
			if(obj->vertical_velocity != 0)
			{
				map_height = map->get_height((int)obj->x >> TILE_SHIFT, (int)obj->y >> TILE_SHIFT);
				if(map_height >= obj->height)
				{
					obj->height = (float)map_height;
					obj->vertical_velocity = 0;
				}
			}
			break;				
		}	

	return 1;
}
	

Object* fire_projectile(Object *shooter, Object *obj_list, int speed, int damage)	// Change to support other projectiles
{
	Object *proj = append_obj(create_object(shooter->x, shooter->y, shooter->height + shooter->h / 2, 
											1, 32, 32, 
											shooter->angle, true, true, PROJECTILE), 
								obj_list);
	
	// Get projectile away from shooter so it doesn't hurt it	(not perfect)
	do
	{
		proj->x += cos(proj->velocity_angle) * (3 + abs(shooter->velocity));
		proj->y += sin(proj->velocity_angle) * (3 + abs(shooter->velocity));		
	}while	(obj_collide(shooter, proj));		
	
	proj->attrib[P_DAMAGE] = damage;
	proj->velocity = speed;	
	
	return proj;
}


void update_objects(Object *obj_list, Map *map, Object *player_obj, float frame_time)
{
	Object *temp = obj_list;	// should be pointer to first obj in list
	for(; temp; temp = temp->next)
	{
		if(temp->type == ENEMY)
		{
			if(temp->attrib[E_FIRE_DELAY] > 0)	temp->attrib[E_FIRE_DELAY] -= int(frame_time * 1000.f);
			// Here if enemy targets were stored, they could fight other things besides the player
			enemy_ai(temp, player_obj, obj_list, map);
		}
			
		if(update_animation(temp, frame_time))	// True if an animation has finished
		{
			if(temp->type == ENEMY)	
			{
				if(temp->state == S_HURT || temp->state == S_ATTACK)	temp->state = S_FOLLOW;
			}
			else if(temp->type == PROJECTILE && temp->state == S_ATTACK) 
			{	
				temp = remove_obj(temp);
				continue;		
			}
		}	

		move_object(temp, obj_list, map, frame_time);			
	}
}