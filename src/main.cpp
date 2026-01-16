#include "main.h"
#include "rlgl.h"

int main()
{
    // Ensure state is zero-initialized
    state = {};

    // Init
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    {
        ASSERT(load_level("../res/level.txt") == 0);

        {
            Texture2D default_tex = LoadTexture("../res/stone.png");
            SetTextureWrap(default_tex, TEXTURE_WRAP_REPEAT);
            for (auto& wall : state.level.walls) wall.texture = default_tex;
            state.ground_texture = LoadTexture("../res/ground.png");
            SetTextureWrap(state.ground_texture, TEXTURE_WRAP_REPEAT);
        }

        // Init camera object
        state.camera.position = (Vector3){ 5.8f, 2.5f, 4.2f };
        state.camera.target = (Vector3){ 6.0f, 2.5f, 4.0f };
        state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        state.camera.fovy = 60.0f;
        state.camera.projection = CAMERA_PERSPECTIVE;

        // Set initial sector and selection
        state.current_sector = 1;
        state.selection = { SELECT_NONE, -1, -1 };
        state.hover = { SELECT_NONE, -1, -1 };
    }

    while (!WindowShouldClose())
    {
        {   // Update
            if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT) && !ImGui::GetIO().WantCaptureMouse)
            {
                if (!IsCursorHidden()) DisableCursor();
                UpdateCamera(&state.camera, CAMERA_FIRST_PERSON);
            }
            else
            {
                if (IsCursorHidden()) EnableCursor();
            }
            state.m_pos = GetMousePosition();

            // Handle mouse click for selection (only when Shift is held)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !ImGui::GetIO().WantCaptureMouse && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
                state.selection = get_selection_from_mouse();
            }

            if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && !ImGui::GetIO().WantCaptureMouse) {
                state.hover = get_selection_from_mouse();
            } else {
                state.hover = { SELECT_NONE, -1, -1 };
            }

            // Update player sector based on current position
            const v2f player_pos = { state.camera.position.x, state.camera.position.z };
            const int new_sector = find_player_sector(player_pos);
            if (new_sector != SECTOR_NONE) {
                state.current_sector = new_sector;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        {   // Draw in 3d World
            BeginMode3D(state.camera);

            // Draw all walls in all sectors
            for (size_t s = 1; s < state.level.sectors.size(); s++) {
                const sector_t* sector = &state.level.sectors[s];

                // Draw floor and ceiling
                if (sector->nwalls >= 3) {
                    const bool is_selected_sector = (state.selection.type == SELECT_SECTOR && state.selection.index == (int)s);
                    const bool is_highlighted = is_selected_sector && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
                    const auto l = static_cast<unsigned char>(255 * sector->light);
                    Color tint = { l, l, l, 255 };
                    if (is_highlighted) {
                        tint = (Color){ 255, 255, 255, 60 }; // Super light white
                    }

                    if (state.ground_texture.id != 0) {
                        rlSetTexture(state.ground_texture.id);
                    }

                    rlBegin(RL_TRIANGLES);
                    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                    for (size_t i = 1; i < sector->nwalls - 1; i++) {
                        const wall_t* w0 = &state.level.walls[sector->firstwall];
                        const wall_t* wi = &state.level.walls[sector->firstwall + i];
                        const wall_t* wi1 = &state.level.walls[sector->firstwall + i + 1];

                        const float v0x  = static_cast<float>(w0->a.x);
                        const float v0z  = static_cast<float>(w0->a.y);
                        const float vix  = static_cast<float>(wi->a.x);
                        const float viz  = static_cast<float>(wi->a.y);
                        const float vi1x = static_cast<float>(wi1->a.x);
                        const float vi1z = static_cast<float>(wi1->a.y);

                        // Floor (facing up)
                        rlNormal3f(0.0f, 1.0f, 0.0f);
                        rlTexCoord2f(v0x, v0z);   rlVertex3f(v0x, sector->zfloor, v0z);
                        rlTexCoord2f(vi1x, vi1z); rlVertex3f(vi1x, sector->zfloor, vi1z);
                        rlTexCoord2f(vix, viz);   rlVertex3f(vix, sector->zfloor, viz);

                        // Ceiling (facing down)
                        rlNormal3f(0.0f, -1.0f, 0.0f);
                        rlTexCoord2f(v0x, v0z);   rlVertex3f(v0x, sector->zceil, v0z);
                        rlTexCoord2f(vix, viz);   rlVertex3f(vix, sector->zceil, viz);
                        rlTexCoord2f(vi1x, vi1z); rlVertex3f(vi1x, sector->zceil, vi1z);
                    }
                    rlEnd();
                    rlSetTexture(0);
                }

                rlBegin(RL_QUADS);
                for (size_t i = 0; i < sector->nwalls; i++) {
                    const wall_t* wall = &state.level.walls[sector->firstwall + i];
                    const int wall_idx = (int)(sector->firstwall + i);

                    const bool is_selected = (state.selection.type == SELECT_WALL && state.selection.index == wall_idx);
                    const bool is_hovered = (state.hover.type == SELECT_WALL && state.hover.index == wall_idx);
                    const bool is_selected_sector = (state.selection.type == SELECT_SECTOR && state.selection.index == (int)s);
                    const bool is_highlighted = (is_selected || is_hovered || is_selected_sector) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));

                    if (wall->portal != 0 && !is_highlighted) continue;

                    {
                        unsigned char l = (unsigned char)(255 * sector->light);
                        Color tint = { l, l, l, 255 };
                        if (is_highlighted) {
                            tint = (Color){ 255, 100, 200, 160 }; // Pink and see-through
                        }
                        rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                        const auto x1 = static_cast<float>(wall->a.x);
                        const auto z1 = static_cast<float>(wall->a.y);
                        const auto x2 = static_cast<float>(wall->b.x);
                        const auto z2 = static_cast<float>(wall->b.y);

                        const float wall_length = sqrtf((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
                        const float wall_height = sector->zceil - sector->zfloor;

                        if (wall_length > 0) {
                            rlNormal3f((z2 - z1) / wall_length, 0.0f, -(x2 - x1) / wall_length);
                        }

                        const float f_h = sector->zfloor;
                        const float c_h = sector->zceil;

                        if (wall->portal == 0 && wall->texture.id != 0) {
                            rlSetTexture(wall->texture.id);
                        } else {
                            rlSetTexture(0);
                        }

                        rlTexCoord2f(wall_length, wall_height); rlVertex3f(x2, f_h, z2);
                        rlTexCoord2f(0.0f, wall_height);        rlVertex3f(x1, f_h, z1);
                        rlTexCoord2f(0.0f, 0.0f);               rlVertex3f(x1, c_h, z1);
                        rlTexCoord2f(wall_length, 0.0f);        rlVertex3f(x2, c_h, z2);

                        rlSetTexture(0);
                    }
                }
                rlEnd();
            }

            EndMode3D();
        }

        {   // ImGui
            rlImGuiBegin();
            constexpr float padding = 10.0f;
            ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(280, (float)GetScreenHeight() - 2 * padding), ImGuiCond_Always);

            if (ImGui::Begin("raycast", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::Text("FPS: %d", GetFPS());
                ImGui::Text("Mouse: %.0f, %.0f", state.m_pos.x, state.m_pos.y);
                ImGui::Text("Camera: %.1f, %.1f, %.1f",
                    state.camera.position.x,
                    state.camera.position.y,
                    state.camera.position.z);
                ImGui::Separator();

                ImGui::Text("Level Info:");
                ImGui::Text("Sectors: %zu", state.level.sectors.size() - 1);
                ImGui::Text("Walls: %zu", state.level.walls.size());
                ImGui::Text("Current Sector: %d", state.current_sector);

                // Display selected object properties
                if (state.selection.type == SELECT_WALL) {
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Selected: Wall #%d", state.selection.index);

                    if (state.selection.index >= 0 && state.selection.index < (int)state.level.walls.size()) {
                        wall_t* wall = &state.level.walls[state.selection.index];

                        ImGui::Text("Position A: (%d, %d)", wall->a.x, wall->a.y);
                        ImGui::Text("Position B: (%d, %d)", wall->b.x, wall->b.y);

                        if (ImGui::Button("Toggle Portal")) {
                            wall->portal = (wall->portal == 0) ? 1 : 0;
                        }

                        // Allow editing wall endpoints
                        ImGui::Text("Edit Wall:");
                        ImGui::DragInt2("Point A", &wall->a.x, 0.1f);
                        ImGui::DragInt2("Point B", &wall->b.x, 0.1f);

                        // Find which sector this wall belongs to
                        for (size_t s = 1; s < state.level.sectors.size(); s++) {
                            const sector_t* sector = &state.level.sectors[s];
                            if (state.selection.index >= (int)sector->firstwall &&
                                state.selection.index < (int)(sector->firstwall + sector->nwalls)) {
                                ImGui::Text("Belongs to Sector: %d", sector->id);
                                if (ImGui::Button("Select Sector")) {
                                    state.selection.type = SELECT_SECTOR;
                                    state.selection.index = s;
                                }
                                break;
                            }
                        }
                    }
                } else if (state.selection.type == SELECT_SECTOR) {
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Selected: Sector #%d", state.selection.index);

                    if (state.selection.index > 0 && state.selection.index < (int)state.level.sectors.size()) {
                        sector_t* sector = &state.level.sectors[state.selection.index];

                        ImGui::Text("Sector ID: %d", sector->id);
                        ImGui::Text("First Wall: %zu", sector->firstwall);
                        ImGui::Text("Wall Count: %zu", sector->nwalls);

                        ImGui::SliderFloat("Floor Height", &sector->zfloor, -5.0f, 10.0f, "%.1f");
                        ImGui::SliderFloat("Ceiling Height", &sector->zceil, 0.0f, 20.0f, "%.1f");
                        ImGui::SliderFloat("Light Level", &sector->light, 0.0f, 1.0f, "%.2f");

                        ImGui::Text("Walls in this sector:");
                        for (size_t i = 0; i < sector->nwalls; i++) {
                            int wall_idx = sector->firstwall + i;
                            if (ImGui::SmallButton(("Wall #" + std::to_string(wall_idx)).c_str())) {
                                state.selection.type = SELECT_WALL;
                                state.selection.index = wall_idx;
                            }
                        }
                    }
                }

                if (state.selection.type != SELECT_NONE) {
                    ImGui::Separator();
                    if (ImGui::Button("Clear Selection")) {
                        state.selection = { SELECT_NONE, -1, -1 };
                    }
                }

                ImGui::End();
            }

            rlImGuiEnd();
        }

        EndDrawing();
    }

    // Cleanup
    cleanup_level();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}