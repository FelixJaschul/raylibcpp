#include "main.h"
#include "rlgl.h"

int main()
{
    // Init
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    {   // Init one wall in level
        state.level.sectors = new sector_t*[MAX_SECT];
        state.level.sectors[0] = new sector_t;
        state.level.sectors[0]->floor_h = 0.0f;
        state.level.sectors[0]->ceil_h  = 5.0f;
        state.level.sectors[0]->walls = new wall_t*[MAX_WALLS];
        state.level.sectors[0]->walls[0] = new wall_t;
        state.level.sectors[0]->walls[0]->w = 10.0f;
        state.level.sectors[0]->walls[0]->texture = LoadTexture("../res/stone.png");
        SetTextureWrap(state.level.sectors[0]->walls[0]->texture, TEXTURE_WRAP_REPEAT);
    }

    {   // Init camera object
        state.camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
        state.camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
        state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        state.camera.fovy = 60.0f;
        state.camera.projection = CAMERA_PERSPECTIVE;
    }


    while (!WindowShouldClose())
    {
        {   // Update
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !ImGui::GetIO().WantCaptureMouse)
            {
                if (!IsCursorHidden()) DisableCursor();
                UpdateCamera(&state.camera, CAMERA_FIRST_PERSON);
            }
            else
            {
                if (IsCursorHidden()) EnableCursor();
            }
            state.m_pos = GetMousePosition();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        {   // Draw in 3d World
            BeginMode3D(state.camera);
            rlBegin(RL_QUADS);
            {
                rlSetTexture(state.level.sectors[0]->walls[0]->texture.id);
                const auto [r, g, b, a] = WHITE;
                rlColor4ub(r, g, b, a);
                rlNormal3f(0.0f, 0.0f, 1.0f);

                const float w_w = state.level.sectors[0]->walls[0]->w;
                const float s_h = state.level.sectors[0]->ceil_h;
                const float x1  = -w_w / 2.0f, x2 = w_w / 2.0f;
                const float f_h = state.level.sectors[0]->floor_h;
                const float c_h = f_h + s_h;

                rlTexCoord2f(0.0f, s_h);  rlVertex3f(x1, f_h, 0.0f);
                rlTexCoord2f(w_w,  s_h);  rlVertex3f(x2, f_h, 0.0f);
                rlTexCoord2f(w_w,  0.0f); rlVertex3f(x2, c_h, 0.0f);
                rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x1, c_h, 0.0f);
                rlSetTexture(0);
            }
            rlEnd();
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
                ImGui::SliderFloat("Wall Width", &state.level.sectors[0]->walls[0]->w, 1.0f, 20.0f, "%.1f");
                ImGui::SliderFloat("Sect Floor Height", &state.level.sectors[0]->floor_h, 0.0f, 20.0f, "%.1f");
                ImGui::SliderFloat("Sect Ceil Height", &state.level.sectors[0]->ceil_h, 0.0f, 20.0f, "%.1f");
                ImGui::End();
            }

            rlImGuiEnd();
        }

        EndDrawing();
    }

    // Cleanup
    {
        UnloadTexture(state.level.sectors[0]->walls[0]->texture);
        delete state.level.sectors[0]->walls[0];
        delete[] state.level.sectors[0]->walls;
        delete state.level.sectors[0];
        delete[] state.level.sectors;
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}