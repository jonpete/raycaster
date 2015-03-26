#include "draw.h"
#include "globals.h"

inline int shade_pixel(int pixel, int light)	//16 bit 565 RGB only
{
	int r = (pixel & 0xf800) >> 11;	
	int g = (pixel & 0x7e0) >> 5;
	int b = (pixel & 0x1f);	
	r = (r * light) >> 5;	
	g = (g * light) >> 5;
	b = (b * light) >> 5;	
	return (r << 11) | (g << 5) | b;	
}


void draw_wall_column(BITMAP* frame, BITMAP* tex, int x, int y, int wall_height, int scaled_height, int tex_offset, int light, int upper_clip, int lower_clip)
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
		*dest = shade_pixel(column[(source_y & tex_h) >> 16], light);

		source_y += delta_y;
		dest += screen_w;
	}while(height--);
}


void draw_floor_line_fixed(BITMAP* frame, BITMAP* texture, int x1, int y1, int x2, int y2, int screen_x, int screen_y, int w, int light)
{
	short* dest = (short*)frame->line[screen_y] + screen_x;
	
	const int delta_x = (x2 - x1) / w;
	const int delta_y = (y2 - y1) / w;
	const int tex_w = texture->w - 1;
	const int tex_h = texture->h - 1;
	
	while(w--)
	{
		*dest = shade_pixel(((short*)texture->line[(y1 >> 16) & tex_h])[(x1 >> 16) & tex_w],light);		
		dest++;

		x1 += delta_x;
		y1 += delta_y;
	} 
}


int calc_light(int light, int dist, int amnt)
{
	/* LIGHT DOESN'T WORK VERY WELL */

	/*if(light >= FULL_BRIGHT || light > 31) return 31;
	if(amnt == 0) return light;
	
	dist += 256;
	int dist_light = amnt / (dist > 0 ? dist : 1);
	dist_light = light + dist_light;
		
	if(dist_light > 31) dist_light = 31;
*/	

	
	//float dist_light = (10000 * ((float)light /24.f)) / (float)dist;

	/* Just distance fade out for now */

	float dist_light = 10000/dist;
	if(dist_light < 0) dist_light = 0;
	if(dist_light >31) dist_light = 31;

	return (int)dist_light;	
}
