#include "esp.h"
#include "gui.h"
#include "memory.h"
#include "../imgui/imgui.h"

#include "globals.h"

#include <vector>
#include <iostream>

extern MemoryManager memMgr;

void esp::Initialize() {
    // Perform any required initialization
    std::cout << "[INFO] ESP system initialized." << std::endl;
}

namespace esp {
    std::vector<Entity> GetEntities() {
        std::vector<Entity> entities;

        for (int i = 0; i < 64; ++i) {  // Replace 64 with the actual max entities
            DWORD64 entityAddress = 0;
            if (!memMgr.Read(memMgr.clientDLLBase + offsets::dwEntityList + i * 0x10, &entityAddress, sizeof(entityAddress)) || entityAddress == 0) {
                continue;
            }

            Entity entity;
            if (!memMgr.Read(entityAddress + offsets::m_vecOrigin, &entity.position, sizeof(entity.position)) ||
                !memMgr.Read(entityAddress + offsets::m_iHealth, &entity.health, sizeof(entity.health)) ||
                !memMgr.Read(entityAddress + offsets::m_iTeamNum, &entity.team, sizeof(entity.team))) {
                continue;
            }

            // Filter: Only include valid player entities
            if (entity.team < 1 || entity.team > 2) continue; // T and CT teams
            if (entity.health <= 0) continue; // Ignore dead players

            entities.push_back(entity);
        }

        return entities;
    }

    bool WorldToScreen(const vec3& worldPos, vec2& screenPos, const float* viewMatrix, int screenWidth, int screenHeight) {
        float w = viewMatrix[3] * worldPos.x + viewMatrix[7] * worldPos.y + viewMatrix[11] * worldPos.z + viewMatrix[15];
        if (w < 0.1f) return false;

        screenPos.x = (viewMatrix[0] * worldPos.x + viewMatrix[4] * worldPos.y + viewMatrix[8] * worldPos.z + viewMatrix[12]) / w;
        screenPos.y = (viewMatrix[1] * worldPos.x + viewMatrix[5] * worldPos.y + viewMatrix[9] * worldPos.z + viewMatrix[13]) / w;

        screenPos.x = (screenWidth / 2.0f) + (screenWidth / 2.0f) * screenPos.x;
        screenPos.y = (screenHeight / 2.0f) - (screenHeight / 2.0f) * screenPos.y;

        return true;
    }


    void esp::Render() {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        auto entities = GetEntities();

        std::cout << "[DEBUG] Number of valid entities: " << entities.size() << std::endl;

        float viewMatrix[16];
        if (!memMgr.Read(memMgr.clientDLLBase + offsets::dwViewMatrix, viewMatrix, sizeof(viewMatrix))) {
            std::cerr << "[ERROR] Failed to read ViewMatrix." << std::endl;
            return;
        }

        for (const auto& entity : entities) {
            vec2 screenPos;
            if (WorldToScreen(entity.position, screenPos, viewMatrix, globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT)) {
                // Example box dimensions
                float boxWidth = 50.0f;
                float boxHeight = 100.0f;

                // Draw the box
                drawList->AddRect(ImVec2(screenPos.x - boxWidth / 2, screenPos.y - boxHeight),
                    ImVec2(screenPos.x + boxWidth / 2, screenPos.y),
                    IM_COL32(255, 0, 0, 255)); // Red color
            }
        }
    }
}
