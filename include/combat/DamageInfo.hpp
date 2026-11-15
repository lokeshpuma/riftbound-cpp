#pragma once

#include <raylib.h>
#include <cstdint>

namespace riftbound {

struct DamageInfo {
    float amount = 0.0f;
    Vector2 knockback = { 0.0f, 0.0f };
    Vector2 sourcePosition = { 0.0f, 0.0f };
    bool isFromPlayer = false;
    uint32_t attackId = 0; // Unique ID to prevent multi-hit within same swing
};

} // namespace riftbound
