#include <iostream>
#include "raylib.h"
#include "imgui.h"
#include "main.h"

int main()
{
    std::cout << "Hello World!" << std::endl;

    // Basic Raylib init test
    InitWindow(WIDTH, HEIGHT, "Test");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello Raylib!", 100, HEIGHT/2, 100, BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}