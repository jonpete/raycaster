#include "guns.h"


Gun guns[NUM_GUNS];

// Regular bullet speeds should be 300+
// Laser/rocket speeds can be slower (100ish)

void init_guns()
{	 
	guns[G_LASER].damage = 10;
	guns[G_LASER].recharge = 5000;
	guns[G_LASER].speed = 50;
	guns[G_LASER].sprite = 1;
	guns[G_LASER].bullet_sprite_w = 32;
	guns[G_LASER].bullet_sprite_h = 32;
	guns[G_LASER].full_bright = true;	
}