#ifndef MAP_H
#define MAP_H

#include <fstream>
#include <string>
#include "globals.h"
#include "mathfunk.h"

class Tile
{
	public:
	int height, ceiling_height;	
	unsigned start_light, tex, light;

	inline int get_tex(int side) const	{ return tex >> side & 0xff;	}
	inline int get_light(int side) const 	{ return light >> side & 0x1f;  }
	
	inline bool compare(const Tile* const t) 
	{ 
		return (t->height == height && t->ceiling_height == ceiling_height
				&& t->light == light 
				&& t->tex == tex); 
	}
	
	Tile() { 		
		light = start_light = 0;		
		tex = 0;
		height = OUT_OF_BOUNDS; }	
};


class Map
{
	private:
	Tile bad_tile;
	Tile* tiles;	
	int w, h;

	public:
	int p_start_x, p_start_y, p_start_h;
	float p_start_angle;
		
	inline Tile* get_tile(int x, int y)	{
		if (y < 0 || x < 0 || y >> TILE_SHIFT >= h || x >> TILE_SHIFT >= w) return &bad_tile;		
		else return &tiles[w * (y >> TILE_SHIFT) + (x >> TILE_SHIFT)];
	}

	inline Tile* get_tile(float x, float y)	{
		if (y < 0.f || x < 0.f || y >= h << TILE_SHIFT || x >= w << TILE_SHIFT) return &bad_tile;		
		else return &tiles[w * ((int)y >> TILE_SHIFT) + ((int)x >> TILE_SHIFT)];
	}

	inline int get_height(int tile_x, int tile_y)
	{
		if (tile_y < 0 || tile_x < 0 || tile_y >= h || tile_x >= w) return OUT_OF_BOUNDS;
		else return tiles[w * tile_y + tile_x].height;
	}

	inline int get_light(int tile_x, int tile_y, int side)  {
		if (tile_y < 0 || tile_x < 0 || tile_y >= h || tile_x >= w) return 0;
		else return tiles[w * tile_y + tile_x].get_light(side);
	}

	int add_light(int light1, int light2)
	{
		if(light1 < 0) light1 = 0;
		
		int light = light1 + light2;			
		return light < 32 ? light : 31;
	}

	// Casts a circle of (max) light out from point
	// Looks kinda cool but causes too many tile differences and slows everything down. Don't use.
	void cast_light(float x, float y, int dist)
	{		
		float start_x = x;
		float start_y = y;
		Tile *tile = get_tile(x, y);		
		
		int light_height = tile->height + (tile->ceiling_height - tile->height) / 2;

		for(float angle = 0; angle < TWO_PI; angle += TWO_PI / 720)
		{
			x = start_x;
			y = start_y;
			
			for(int i = 1; i < dist; i ++)
			{
				
				if(i >= dist) break;

				int north = get_light(x / TILE_SIZE, y / TILE_SIZE, L_NORTH);
				int south = get_light(x / TILE_SIZE, y / TILE_SIZE, L_SOUTH);
				int east = get_light(x / TILE_SIZE, y / TILE_SIZE, L_EAST);
				int west = get_light(x / TILE_SIZE, y / TILE_SIZE, L_WEST);
				int floor = get_light(x / TILE_SIZE, y / TILE_SIZE, L_FLOOR);
				int ceiling = get_light(x / TILE_SIZE, y / TILE_SIZE, L_CEILING);

				int amnt = 1;				
				floor = add_light(amnt, floor);
				ceiling = add_light(amnt, ceiling);
				
				if(cos(angle) > 0)	west = add_light(amnt, west);
				else if(cos(angle) < 0)		east = add_light(amnt, east);
				
				if(sin(angle) > 0)	north = add_light(amnt, north);
				else if(sin(angle) < 0)	south = add_light(amnt, south);

				tiles[(int)y / TILE_SIZE * w + (int)x / TILE_SIZE].start_light = 
				tiles[(int)y / TILE_SIZE * w + (int)x / TILE_SIZE].light =
					(north << L_NORTH) | (south << L_SOUTH) | (east << L_EAST) | (west << L_WEST) |
					(floor << L_FLOOR) | (ceiling << L_CEILING);

				float new_x = x;
				float new_y = y;

				while((int)x / TILE_SIZE == (int)new_x / TILE_SIZE && (int)y / TILE_SIZE == (int)new_y / TILE_SIZE && i < dist)
				{
					if((int)x < 0 || (int)y < 0 || (int)x >= w * TILE_SIZE || (int)y >= h * TILE_SIZE)
					{
						i = dist;
						break;
					}
					
					tile = get_tile(x, y);
					if(tile->height > light_height || tile->ceiling_height < light_height)
					{
						i = dist;
						break;
					}
					x += cos(angle) * 2;
					y += sin(angle) * 2;
					i ++;

				}
			}
		}
	}
	

	void load(std::string filename)
	{				
		bad_tile.height = OUT_OF_BOUNDS;
		std::ifstream l_file(filename);
		if (!l_file) return;
		int temp[4] = {0,0,0,0};
		int tex;
		int north = 1;
		int south = 1;
		int east = 1;
		int west = 1;
		int floor = 1;
		int ceiling = 1;
		int light = (east << L_EAST) | (north << L_NORTH) | (ceiling << L_CEILING) | (west << L_WEST) | (south << L_SOUTH) | (floor << L_FLOOR);

		l_file >> w;
		l_file >> h;
		tiles = new Tile [w * h];

		std::string buffer;		
		while(buffer != "end" && !l_file.eof())
		{
			getline(l_file, buffer, '\n');
			if(buffer == "texture")
			{
				for(int i = 0; i < 4; i++)
				{
					getline(l_file, buffer, (i < 3)? ',' : '\n');
					temp[i] = std::stoi(buffer);
				}
				tex = (temp[0] << T_LOWER) | (temp[1] << T_UPPER) | (temp[2] << T_FLOOR) | (temp[3] << T_CEILING);
			}
			else if(buffer == "height")
			{
				getline(l_file, buffer, ',');
				floor = std::stoi(buffer);
				getline(l_file, buffer, '\n');
				ceiling = std::stoi(buffer);
			}
			else if(buffer == "rect")
			{
				int s_x, s_y, s_w, s_h;
				getline(l_file, buffer, ',');
				s_x = std::stoi(buffer);
				getline(l_file, buffer, ',');
				s_y = std::stoi(buffer);
				getline(l_file, buffer, ',');
				s_w = std::stoi(buffer);
				getline(l_file, buffer, '\n');
				s_h = std::stoi(buffer);
				for(int i = s_x; i < s_w + 1; i++)
				{
					for(int j = s_y; j < s_h + 1; j++) 
					{
						tiles[j * w + i].height = floor;
						tiles[j * w + i].ceiling_height = ceiling;
						tiles[j * w + i].tex = tex;
						tiles[j * w + i].light = tiles[j * w + i].start_light = light;
					}
				}
			}
			else if(buffer == "player_tile")
			{
				getline(l_file, buffer, ',');
				p_start_x = std::stoi(buffer);
				getline(l_file, buffer, ',');
				p_start_y = std::stoi(buffer);
				p_start_h = tiles[p_start_y * w + p_start_x].height;
				getline(l_file, buffer, '\n');
				p_start_angle = std::stof(buffer);
				p_start_x = p_start_x * TILE_SIZE + TILE_SIZE/2;
				p_start_y = p_start_y * TILE_SIZE + TILE_SIZE/2;
			}
			else if(buffer == "circle_light")	// Buggy as poop?
			{
				getline(l_file, buffer, ',');
				float x = std::stof(buffer);
				getline(l_file, buffer, ',');
				float y = std::stof(buffer);
				getline(l_file, buffer, '\n');
				int dist = std::stoi(buffer);
				cast_light(x * TILE_SIZE, y * TILE_SIZE, dist);
			}
			else if(buffer == "set_light")
			{
				getline(l_file, buffer, ',');
				north = south = east = west = std::stoi(buffer);
				getline(l_file, buffer, ',');
				floor = std::stoi(buffer);
				getline(l_file, buffer, '\n');
				ceiling = std::stoi(buffer);
				light = (east << L_EAST) | (north << L_NORTH) | (ceiling << L_CEILING) | (west << L_WEST) | (south << L_SOUTH) | (floor << L_FLOOR);
			}
		}
		
		l_file.close();		
	}	

	~Map() { delete[] tiles; }
};

#endif