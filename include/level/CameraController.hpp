#pragma once

#include "core/Types.hpp"
#include <raylib.h>

namespace riftbound {

class CameraController {
public:
    CameraController(int viewportWidth, int viewportHeight);

    void SetBounds(float minX, float minY, float maxX, float maxY);
    void Update(float dt, Vector2 targetPos, Direction targetFacing);
    void SnapTo(Vector2 targetPos);

    Camera2D GetRaylibCamera() const;
    Vector2 GetPosition() const { return m_camera.target; }

private:
    Camera2D m_camera{};
    int m_viewportWidth;
    int m_viewportHeight;

    float m_minX = 0.0f;
    float m_minY = 0.0f;
    float m_maxX = 2000.0f;
    float m_maxY = 1000.0f;

    float m_lookAheadDistance = 40.0f;
    float m_smoothSpeed = 6.0f;
};

} // namespace riftbound
