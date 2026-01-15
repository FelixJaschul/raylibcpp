#include "main.h"
#include "rlgl.h"

int main()
{
    // Init
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    DisableCursor();

    {
        state.level.walls = new wall_t*[MAX_WALLS];
        state.level.walls[0] = new wall_t;
        state.level.walls[0]->texture = LoadTexture("../res/stone.png");;
    }

    {
        state.camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
        state.camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
        state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        state.camera.fovy = 60.0f;
        state.camera.projection = CAMERA_PERSPECTIVE;
    }


    while (!WindowShouldClose())
    {
        {   // Update
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsCursorHidden()) DisableCursor();
            UpdateCamera(&state.camera, CAMERA_FIRST_PERSON);
            state.m_pos = GetMousePosition();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        {   // Draw in 3d World
            BeginMode3D(state.camera);
            rlSetTexture(state.level.walls[0]->texture.id);
            const auto [r, g, b, a] = WHITE;
            rlBegin(RL_QUADS);
            rlColor4ub(r, g, b, a);
            rlNormal3f(0.0f, 0.0f, 1.0f);
            
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-5.0f, 0.0f, 0.0f);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f( 5.0f, 0.0f, 0.0f);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f( 5.0f, 5.0f, 0.0f);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-5.0f, 5.0f, 0.0f);
            
            rlEnd();

            rlSetTexture(0);
            EndMode3D();
        }

        {   // ImGui
            rlImGuiBegin();
            constexpr float padding = 10.0f;
            ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(200, (float)GetScreenHeight() - 2 * padding), ImGuiCond_Always);

            if (ImGui::Begin("raycast", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::Text("FPS: %d", GetFPS());
                ImGui::Text("Mouse: %.0f, %.0f", state.m_pos.x, state.m_pos.y);
                ImGui::Text("Camera: %.1f, %.1f, %.1f", state.camera.position.x, state.camera.position.y, state.camera.position.z);
                ImGui::Text("Looking at: Nothing");
                ImGui::End();
            }

            rlImGuiEnd();
        }

        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < MAX_WALLS; i++) {
        delete state.level.walls[i];
        UnloadTexture(state.level.walls[i]->texture);
    }
    delete[] state.level.walls;

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}