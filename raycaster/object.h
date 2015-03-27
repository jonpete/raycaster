#ifndef OBJECT_H
#define OBJECT_H

#include "globals.h"
#include "map.h"

#define NUM_ATTRIB 4

enum anim_state {A_DEFAULT = 0, A_ATTACK = 1, A_HURT = 2, A_DEAD = 3};
enum state {S_DEFAULT, S_ATTACK, S_HURT, S_FOLLOW, S_JUMP, S_DEAD};
enum obj_type {BLOCK, PLAYER, ENEMY, PASS, PROJECTILE};

// For attribute indexes
enum {E_HEALTH = 0, E_WEAPON = 1, E_FIRE_DELAY = 2};
enum {P_DAMAGE = 0, P_GLOW = 0};

class Object
{
	public:
	int sprite, w, h, distance;	
	float x, y, height;
	float angle_to_view, angle;	
	float velocity, velocity_angle, vertical_velocity;

	int frame;	
	bool is_animated;
	bool has_directions;
	
	anim_state anim;
	state state;
	obj_type type;
	int attrib[NUM_ATTRIB];		// Enemy/player/pickup/etc specific stuff

	Object *prev, *next;

	static int total_num_objs;

	Object() { prev = 0; next = 0;}
    static bool obj_ptr_comp(Object *obj1, Object *obj2)	{ 	return obj1->distance > obj2->distance; }
};


Object* remove_obj(Object *obj);

Object* append_obj(Object *obj, Object *obj_list);

Object* create_object(float x, float y, float height, int sprite, int w, int h, float angle, bool has_directions, bool is_animated, obj_type type, int *attrib = 0);

bool start_obj_anim(Object *obj, anim_state anim, bool safe = true);

bool obj_collide(Object *obj1, Object *obj2);

int test_obj_map_collisions(Object *obj, Map *map, int allowable_height = 0);

Object* test_all_obj_collisions(Object *obj, Object *obj_list);

int update_animation(Object *obj, float frame_time);

int move_object(Object *obj, Object *obj_list, Map *map, float frame_time);	

Object* fire_projectile(Object *shooter, Object *obj_list, int speed, int damage);

void update_objects(Object *obj_list, Map *map, Object *player_obj, float frame_time);

#endif