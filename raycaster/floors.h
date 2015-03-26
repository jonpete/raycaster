#ifndef FLOORS_H
#define FLOORS_H

#define NUM_FLOORLINES 128

class Floorline
{	
	public:
	int x, y, w, texture;
	int height;
	Floorline() { x = y = w = texture = height = 0; }	
};


class Floormap	
{
	public:
	int size;	
	Floorline floorlines[NUM_FLOORLINES];
};


extern __forceinline void add_to_floors(Floormap *fm, int x, int y, int height, int texture);

void clear_floors(Floormap *fm, int size);


#endif