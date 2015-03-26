#ifndef GUNS_H
#define GUNS_H


class Gun
{
	public:
	int damage;
	int recharge;
	int speed;
	int sprite;
	int bullet_sprite_w;
	int bullet_sprite_h;
	bool full_bright;
};

enum {G_LASER, NUM_GUNS};

extern Gun guns[NUM_GUNS];

void init_guns();

#endif