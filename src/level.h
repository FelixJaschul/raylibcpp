#ifndef RAY_LEVEL_H
#define RAY_LEVEL_H

#include "main.h"

// Level lifecycle
int init_level();
void deinit_level();
void update_level();
void render_level();

// Level loading functions
int load_level(const char* path);
int save_level(const char* path);
void cleanup_level();

// Utility functions
bool point_in_sector(const sector_t* sector, v2f p);
int find_player_sector(v2f pos);
float point_side(v2f p, v2f a, v2f b);

// Raycast and selection functions
bool ray_intersects_wall(Vector3 ray_origin, Vector3 ray_dir, const wall_t* wall, const sector_t* sector, float* out_distance);
selection_t get_selection_from_mouse();

#endif // RAY_LEVEL_H