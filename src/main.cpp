#include <iostream>
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "main.h"

int main()
{
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}