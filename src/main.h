#ifndef RAY_MAIN_H
#define RAY_MAIN_H

#include <iostream>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#define WIDTH 1200
#define HEIGHT 800
#define MAX_WALLS 10
#define MAX_SECT  10

#define ASSERT(x) do { if(!(x)) std::cout << "Assertion failed: " << #x << std::endl; } while(0)
#define LOG(x) do { std::cout << x << std::endl; } while(0)

#define MAX(a, b) (( (a) > (b) ) ? (a) : (b))
#define MIN(a, b) (( (a) < (b) ) ? (a) : (b))

typedef struct {
    float w;
    Texture2D texture;
} wall_t;

typedef struct {
    float floor_h, ceil_h;
    wall_t **walls;
} sector_t;

typedef struct {
    sector_t **sectors;
} level_t;

typedef struct {
    level_t level;
    Vector2 m_pos;
    Camera3D camera;
} state_t;

inline state_t state;

#endif // RAY_MAIN_H