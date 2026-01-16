#ifndef RAY_MAIN_H
#define RAY_MAIN_H

#include <iostream>
#include <vector>
#include <string>
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

#define WIDTH 1200
#define HEIGHT 800
#define MAX_WALLS 128
#define MAX_SECT  128
#define SECTOR_NONE 0

#define ASSERT(x) do { if(!(x)) std::cout << "Assertion failed: " << #x << std::endl; } while(0)
#define LOG(x) do { std::cout << x << std::endl; } while(0)

#define MAX(a, b) (( (a) > (b) ) ? (a) : (b))
#define MIN(a, b) (( (a) < (b) ) ? (a) : (b))

// 2D vector for level geometry
typedef struct {
    int x, y;
} v2i;

typedef struct {
    float x, y;
} v2f;

// Wall structure with portal support
typedef struct {
    v2i a, b;           // Wall endpoints
    int portal;         // Portal to another sector (0 = solid wall)
    Texture2D texture;  // Wall texture
} wall_t;

// Sector structure
typedef struct {
    int id;             // Sector ID
    size_t firstwall;   // Index of first wall in global wall array
    size_t nwalls;      // Number of walls in this sector
    float zfloor;       // Floor height
    float zceil;        // Ceiling height
    float light;        // Light level (0.0 to 1.0)
} sector_t;

// Level structure
typedef struct {
    std::vector<sector_t> sectors;
    std::vector<wall_t> walls;
} level_t;

// Selection types
enum SelectionType {
    SELECT_NONE,
    SELECT_WALL,
    SELECT_SECTOR
};

typedef struct {
    SelectionType type;
    int index;          // Index of selected wall or sector
    int wall_index;     // For sectors, which wall was clicked
} selection_t;

typedef struct {
    level_t level;
    Vector2 m_pos;
    Camera3D camera;
    int current_sector;  // Which sector the player is in
    selection_t selection;
    selection_t hover;
    Texture2D ground_texture;
} state_t;

inline state_t state;

// Level loading functions
int load_level(const char* path);
int save_level(const char* path);
void cleanup_level();

// Main loop functions
void update();

// Utility functions
bool point_in_sector(const sector_t* sector, v2f p);
int find_player_sector(v2f pos);
float point_side(v2f p, v2f a, v2f b);

// Raycast and selection functions
bool ray_intersects_wall(Vector3 ray_origin, Vector3 ray_dir, const wall_t* wall, const sector_t* sector, float* out_distance);
selection_t get_selection_from_mouse();

#endif // RAY_MAIN_H