#include "main.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>
#include <cmath>
#include <limits>

// Calculate which side of a line a point is on
// Returns: < 0 right, 0 on line, > 0 left
float point_side(v2f p, v2f a, v2f b) {
    return -((p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x));
}

// Check if a point is inside a sector
// Point is inside if it's on the left side of all walls
bool point_in_sector(const sector_t* sector, v2f p) {
    for (size_t i = 0; i < sector->nwalls; i++) {
        const wall_t* wall = &state.level.walls[sector->firstwall + i];

        const v2f a = { static_cast<float>(wall->a.x), static_cast<float>(wall->a.y) };
        const v2f b = { static_cast<float>(wall->b.x), static_cast<float>(wall->b.y) };

        if (point_side(p, a, b) > 0) {
            return false;
        }
    }
    return true;
}

// Find which sector a position is in using BFS
int find_player_sector(v2f pos) {
    if (state.level.sectors.empty()) return SECTOR_NONE;

    // BFS queue
    constexpr int QUEUE_MAX = 64;
    int queue[QUEUE_MAX] = { state.current_sector };
    int i = 0, n = 1;
    bool visited[MAX_SECT];
    std::memset(visited, 0, sizeof(visited));

    while (n > 0) {
        const int id = queue[i];
        i = (i + 1) % QUEUE_MAX;
        n--;

        if (id <= 0 || id >= static_cast<int>(state.level.sectors.size())) continue;
        if (visited[id]) continue;
        visited[id] = true;

        const sector_t* sector = &state.level.sectors[id];

        if (point_in_sector(sector, pos)) {
            return id;
        }

        // Check neighboring sectors through portals
        for (size_t j = 0; j < sector->nwalls; j++) {
            const wall_t* wall = &state.level.walls[sector->firstwall + j];

            if (wall->portal && !visited[wall->portal]) {
                if (n == QUEUE_MAX) {
                    LOG("Warning: BFS queue full");
                    break;
                }
                queue[(i + n) % QUEUE_MAX] = wall->portal;
                n++;
            }
        }
    }

    return SECTOR_NONE;
}

// Ray-wall intersection test
bool ray_intersects_wall(const Vector3 ray_origin, const Vector3 ray_dir, const wall_t* wall, const sector_t* sector, float* out_distance) {
    // Convert wall points to 3D
    const Vector3 p1 = { static_cast<float>(wall->a.x), sector->zfloor, static_cast<float>(wall->a.y) };
    const Vector3 p2 = { static_cast<float>(wall->b.x), sector->zfloor, static_cast<float>(wall->b.y) };
    const Vector3 p3 = { static_cast<float>(wall->b.x), sector->zceil, static_cast<float>(wall->b.y) };
    const Vector3 p4 = { static_cast<float>(wall->a.x), sector->zceil, static_cast<float>(wall->a.y) };

    // Test intersection with two triangles that make up the wall quad
    const RayCollision hit1 = GetRayCollisionTriangle(
        (Ray){ ray_origin, ray_dir },
        p1, p2, p3
    );

    const RayCollision hit2 = GetRayCollisionTriangle(
        (Ray){ ray_origin, ray_dir },
        p1, p3, p4
    );

    // Return closest hit
    if (hit1.hit && hit2.hit) {
        *out_distance = fminf(hit1.distance, hit2.distance);
        return true;
    }

    if (hit1.hit) {
        *out_distance = hit1.distance;
        return true;
    }

    if (hit2.hit) {
        *out_distance = hit2.distance;
        return true;
    }

    return false;
}

// Get what the mouse is pointing at
selection_t get_selection_from_mouse() {
    selection_t result = { SELECT_NONE, -1, -1 };

    auto [position, direction] = GetScreenToWorldRay(state.m_pos, state.camera); // ray

    float closest_distance = std::numeric_limits<float>::max();
    int closest_sector = -1;
    int closest_wall = -1;

    // Check all sectors
    for (size_t s = 1; s < state.level.sectors.size(); s++) {
        const sector_t* sector = &state.level.sectors[s];

        // Check all walls in this sector
        for (size_t i = 0; i < sector->nwalls; i++) {
            const wall_t* wall = &state.level.walls[sector->firstwall + i];

            float distance;
            if (ray_intersects_wall(position, direction, wall, sector, &distance)) {
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_sector = s;
                    closest_wall = sector->firstwall + i;
                }
            }
        }
    }

    if (closest_wall != -1) {
        result.type = SELECT_WALL;
        result.index = closest_wall;
        result.wall_index = closest_wall;
    }

    return result;
}

// Load level from text file
int load_level(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG("Error: Could not open level file: " << path);
        return -1;
    }

    // Clear existing level data
    state.level.sectors.clear();
    state.level.walls.clear();

    // Sector 0 is reserved as "no sector"
    sector_t zero_sector = {};
    state.level.sectors.push_back(zero_sector);

    enum { SCAN_NONE, SCAN_SECTOR, SCAN_WALL } scan_state = SCAN_NONE;

    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;

        // Trim leading whitespace
        size_t start = 0;
        while (start < line.length() && std::isspace(line[start])) {
            start++;
        }

        if (start >= line.length()) continue;  // Empty line
        if (line[start] == '#') continue;      // Comment

        // Check for section headers
        if (line[start] == '[') {
            size_t end = line.find(']', start);
            if (end == std::string::npos) {
                LOG("Error: Malformed section header at line " << line_num);
                return -2;
            }

            std::string section = line.substr(start + 1, end - start - 1);

            if (section == "SECTOR") scan_state = SCAN_SECTOR;
            else if (section == "WALL") scan_state = SCAN_WALL;
            else {
                LOG("Error: Unknown section '" << section << "' at line " << line_num);
                return -3;
            }
            continue;
        }

        // Parse data based on current section
        std::istringstream iss(line.substr(start));

        switch (scan_state) {
            case SCAN_WALL: {
                wall_t wall = {};
                if (!(iss >> wall.a.x >> wall.a.y >> wall.b.x >> wall.b.y >> wall.portal)) {
                    LOG("Error: Invalid wall data at line " << line_num);
                    return -4;
                }
                wall.texture = {0};  // Initialize texture as empty
                state.level.walls.push_back(wall);
                break;
            }

            case SCAN_SECTOR: {
                sector_t sector = {};
                if (!(iss >> sector.id >> sector.firstwall >> sector.nwalls >> sector.zfloor >> sector.zceil >> sector.light)) {
                    LOG("Error: Invalid sector data at line " << line_num);
                    return -5;
                }
                state.level.sectors.push_back(sector);
                break;
            }

            case SCAN_NONE:
                LOG("Error: Data outside of section at line " << line_num);
                return -6;
        }
    }

    file.close();

    LOG("Loaded " << state.level.sectors.size() << " sectors with " << state.level.walls.size() << " walls");

    return 0;
}

// Clean up level resources
void cleanup_level() {
    // Unload all wall textures
    for (const auto& wall : state.level.walls) {
        if (wall.texture.id != 0) {
            UnloadTexture(wall.texture);
        }
    }

    if (state.ground_texture.id != 0) {
        UnloadTexture(state.ground_texture);
    }

    state.level.sectors.clear();
    state.level.walls.clear();
}