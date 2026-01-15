#include "main.h"

int main()
{
    // Init
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    state.level.walls = new wall_t*[MAX_WALLS];
    state.level.walls[0] = new wall_t;
    state.level.walls[0]->texture = LoadTexture("../res/stone.png");
    state.level.walls[0]->texture.width  = 400;
    state.level.walls[0]->texture.height = 400;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKPURPLE);

        DrawTexture(state.level.walls[0]->texture, 250, 50, WHITE);

        // Update
        state.m_pos = GetMousePosition();

        rlImGuiBegin();
        constexpr float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, (float)GetScreenHeight() - 2 * padding), ImGuiCond_Always);
        
        if (ImGui::Begin("raycast", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text("FPS: %d", GetFPS());
            ImGui::Text("Mouse: %.0f, %.0f", state.m_pos.x, state.m_pos.y);
            ImGui::Text("Looking at: Nothing");
            ImGui::End();
        }

        rlImGuiEnd();
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(state.level.walls[0]->texture);
    delete state.level.walls[0];
    delete[] state.level.walls;

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}