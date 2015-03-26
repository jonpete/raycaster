#ifndef FLOORS_H
#define FLOORS_H

#define NUM_FLOORLINES 128

class Floorline
{	
	public:
	int x, y, w, texture;
	int height, light;
	Floorline() { x = y = w = texture = light = height = 0; }	
};


class Floormap	
{
	public:
	int size;	
	Floorline floorlines[NUM_FLOORLINES];
};


extern __forceinline void add_to_floors(Floormap *fm, int x, int y, int height, int texture, int light);

void clear_floors(Floormap *fm, int size);


#endif