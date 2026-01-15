#ifndef RAY_MAIN_H
#define RAY_MAIN_H

#include <iostream>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#define WIDTH 1200
#define HEIGHT 800
#define MAX_WALLS 10

#define ASSERT(x) do { if(!(x)) std::cout << "Assertion failed: " << #x << std::endl; } while(0)
#define LOG(x) do { std::cout << x << std::endl; } while(0)


typedef struct {
    int w, h;
    Texture2D texture;
} wall_t;

typedef struct {
    wall_t **walls;
} level_t;

typedef struct {
    level_t level;
    Vector2 m_pos;
    Camera3D camera;
} state_t;

inline state_t state;

#endif // RAY_MAIN_H