#include "draw.h"
#include "globals.h"


void draw_wall_column(BITMAP* frame, BITMAP* tex, int x, int y, int wall_height, int scaled_height, int tex_offset, int upper_clip, int lower_clip)
{	
	if(scaled_height <= 0) return;
	// Increase fixed point shifts to 20 for more accuracy
	const short* const column = (short*)tex->line[tex_offset];
	const int screen_w = frame->w;
	const int tex_h = (tex->w - 1) << 16;
	const int delta_y = (wall_height << 16) / scaled_height;

	int source_y = (wall_height > tex->w ? wall_height - tex->w : tex->w - wall_height) << 16;	
	int bottom = y + scaled_height;

	if(bottom > lower_clip) bottom = lower_clip;
	if(y < upper_clip)
	{		
		source_y += delta_y * (upper_clip - y);
		y = upper_clip;
	}
	if(bottom < y) return;	
	
	short* dest = (short*)frame->line[y] + x;
	int height = bottom - y;
	do
	{
		*dest = column[(source_y & tex_h) >> 16];

		source_y += delta_y;
		dest += screen_w;
	}while(height--);
}


void draw_floor_line_fixed(BITMAP* frame, BITMAP* texture, int x1, int y1, int x2, int y2, int screen_x, int screen_y, int w)
{
	short* dest = (short*)frame->line[screen_y] + screen_x;
	
	const int delta_x = (x2 - x1) / w;
	const int delta_y = (y2 - y1) / w;
	const int tex_w = texture->w - 1;
	const int tex_h = texture->h - 1;
	
	while(w--)
	{
		*dest = ((short*)texture->line[(y1 >> 16) & tex_h])[(x1 >> 16) & tex_w];		
		dest++;

		x1 += delta_x;
		y1 += delta_y;
	} 
}



