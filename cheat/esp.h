#pragma once
#include <Windows.h>
#include "globals.h"

struct vec3 {
    float x, y, z;
};

struct vec2 {
    float x, y;
};

struct Entity {
    vec3 position;
    int health;
    int team;
};

namespace esp {
    void Initialize();
    void Render();
    bool WorldToScreen(const vec3& worldPos, vec2& screenPos, const float* viewMatrix, int screenWidth, int screenHeight);
}