#ifndef RAY_MAIN_H
#define RAY_MAIN_H

#include <iostream>
#include <vector>
#include <string>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "rlImGui.h"
#include "imgui.h"

#define WIDTH 1200
#define HEIGHT 800
#define PLAYER_HEIGHT 2.0f
#define MAX_WALLS 128
#define MAX_SECT  128
#define SECTOR_NONE 0

#define ASSERT(x) do { if(!(x)) std::cout << "Assertion failed: " << #x << std::endl; } while(0)
#define LOG(x) do { std::cout << x << std::endl; } while(0)

#define MAX(a, b) (( (a) > (b) ) ? (a) : (b))
#define MIN(a, b) (( (a) < (b) ) ? (a) : (b))

typedef struct {
    int x, y;
} v2i;

typedef struct {
    float x, y;
} v2f;

typedef struct {
    v2i a, b;
    int portal;
    Texture2D texture;
} wall_t;

typedef struct {
    int id;
    size_t firstwall;
    size_t nwalls;
    float zfloor;
    float zceil;
    float light;
} sector_t;

typedef struct {
    std::vector<sector_t> sectors;
    std::vector<wall_t> walls;
} level_t;

enum SelectionType {
    SELECT_NONE,
    SELECT_WALL,
    SELECT_SECTOR
};

typedef struct {
    SelectionType type;
    int index;
    int wall_index;
} selection_t;

typedef struct {
    level_t level;
    Vector2 m_pos;
    Camera3D camera;
    int current_sector;
    selection_t selection;
    selection_t hover;
    Texture2D ground_texture;
} state_t;

inline state_t state;

#endif // RAY_MAIN_H