#include "main.h"
#include "level.h"
#include "ui.h"

void init()
{
    InitWindow(WIDTH, HEIGHT, "raycast");
    SetTargetFPS(60);

    init_ui();
    init_level();

    state.camera.position = (Vector3){ 5.8f, 2.5f, 4.2f };
    state.camera.target = (Vector3){ 6.0f, 2.5f, 4.0f };
    state.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    state.camera.fovy = 60.0f;
    state.camera.projection = CAMERA_PERSPECTIVE;

    state.current_sector = 1;
    state.selection = { SELECT_NONE, -1, -1 };
    state.hover = { SELECT_NONE, -1, -1 };
}

void update()
{
    const bool is_shifting = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

    if (!ImGui::GetIO().WantCaptureMouse)
    {
        state.m_pos = GetMousePosition();

        if (!is_shifting) {
            if (!IsCursorHidden()) DisableCursor();
            UpdateCamera(&state.camera, CAMERA_FIRST_PERSON);
        }   else if (IsCursorHidden()) EnableCursor();

        if (is_shifting) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) state.selection = get_selection_from_mouse();
            state.hover = get_selection_from_mouse();
        }   else state.hover = { SELECT_NONE, -1, -1 };
    }

    update_level();
    update_ui();
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);
        render_level();
        render_ui();
    EndDrawing();
}

void deinit()
{
    deinit_level();
    deinit_ui();
    CloseWindow();
}

int main()
{
    state = {};
    init();
    while (!WindowShouldClose())
    {
        update();
        render();
    }
    deinit();
    return 0;
}