#include "floors.h"


__forceinline void add_to_floors(Floormap *fm, int x, int y, int height, int texture)
{
	Floorline* fl = &fm[y].floorlines[fm[y].size - 1];	// Dangerous? Can be -1
	
	if(false || fm[y].size == 0 || x > fl->x + fl->w 		
		|| fl->height != height
		|| fl->texture != texture
		)			
	{
		if(fm[y].size >= NUM_FLOORLINES) return;
		fm[y].size++;		
		fl++;		
		fl->w = 0;
		fl->x = x;
		fl->y = y;
		fl->texture = texture;
		fl->height = height;
	}		
	
	fl->w++;	
}


void clear_floors(Floormap *fm, int size)
{
	for(int i = 0; i < size; i++)	fm[i].size = 0;
}
