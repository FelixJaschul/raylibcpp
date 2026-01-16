#include "ui.h"
#include "level.h"

void init_ui()
{
    rlImGuiSetup(true);
}

void deinit_ui()
{
    rlImGuiShutdown();
}

void update_ui()
{
    // Placeholder for any future UI update logic that isn't inside the render block
}

void render_ui()
{
    rlImGuiBegin();
    constexpr float padding = 10.0f;
    ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(280, static_cast<float>(GetScreenHeight()) - 2 * padding), ImGuiCond_Always);

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

        if (ImGui::Button("Save Level")) ASSERT(save_level("../res/level.txt") == 0);

        // Display selected object properties
        if (state.selection.type == SELECT_WALL)
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Selected: Wall #%d", state.selection.index);

            if (state.selection.index >= 0 && state.selection.index < (int)state.level.walls.size()) {
                wall_t* wall = &state.level.walls[state.selection.index];

                if (ImGui::Button("Toggle Portal")) wall->portal = (wall->portal == 0) ? 1 : 0;
                ImGui::Text("Edit Wall:"); ImGui::DragInt2("Point A", &wall->a.x, 0.1f); ImGui::DragInt2("Point B", &wall->b.x, 0.1f);

                // Find which sector this wall belongs to
                for (size_t s = 1; s < state.level.sectors.size(); s++) {
                    if (const sector_t* sector = &state.level.sectors[s];
                        state.selection.index >= (int)sector->firstwall &&
                        state.selection.index < (int)(sector->firstwall + sector->nwalls))
                    {
                        ImGui::Text("Belongs to Sector: %d", sector->id);
                        if (ImGui::Button("Select Sector")) {
                            state.selection.type = SELECT_SECTOR;
                            state.selection.index = (int)s;
                        }
                        break;
                    }
                }
            }
        }
        else if (state.selection.type == SELECT_SECTOR)
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Selected: Sector #%d", state.selection.index);

            if (state.selection.index > 0 &&
                state.selection.index < static_cast<int>(state.level.sectors.size()))
            {
                sector_t* sector = &state.level.sectors[state.selection.index];

                ImGui::Text("Sector ID: %d", sector->id);
                ImGui::Text("First Wall: %zu", sector->firstwall);
                ImGui::Text("Wall Count: %zu", sector->nwalls);

                ImGui::SliderFloat("Floor Height", &sector->zfloor, -5.0f, 10.0f, "%.1f");
                ImGui::SliderFloat("Ceiling Height", &sector->zceil, 0.0f, 20.0f, "%.1f");
                ImGui::SliderFloat("Light Level", &sector->light, 0.0f, 1.0f, "%.2f");

                ImGui::Text("Walls in this sector:");
                for (size_t i = 0; i < sector->nwalls; i++)
                {
                    if (const int wall_idx = (int)(sector->firstwall + i);
                        ImGui::SmallButton(("Wall #" + std::to_string(wall_idx)).c_str())) {
                        state.selection.type = SELECT_WALL;
                        state.selection.index = wall_idx;
                    }
                }
            }
        }

        if (state.selection.type != SELECT_NONE) ImGui::Separator();
        ImGui::End();
    }

    rlImGuiEnd();
}