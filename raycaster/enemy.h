#ifndef ENEMY_H
#define ENEMY_H

#include "object.h"

int enemy_los(Object *enemy, Object *target, Map *map);

void enemy_follow(Object *enemy, Object *target);

void enemy_ai(Object *enemy, Object *target, Object *obj_list, Map *map);

void kill_enemy(Object *enemy);

#endif