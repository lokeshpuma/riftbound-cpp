#include "level/CameraController.hpp"
#include "core/ParticleSystem.hpp"
#include <algorithm>
#include <cmath>

namespace riftbound {

CameraController::CameraController(int viewportWidth, int viewportHeight)
    : m_viewportWidth(viewportWidth),
      m_viewportHeight(viewportHeight) {
    m_camera.offset = { viewportWidth * 0.5f, viewportHeight * 0.5f };
    m_camera.target = { 0.0f, 0.0f };
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void CameraController::SetBounds(float minX, float minY, float maxX, float maxY) {
    m_minX = minX;
    m_minY = minY;
    m_maxX = maxX;
    m_maxY = maxY;
}

void CameraController::SnapTo(Vector2 targetPos) {
    m_camera.target = targetPos;
}

void CameraController::Update(float dt, Vector2 targetPos, Direction targetFacing) {
    // Look-ahead based on facing direction
    float lookAheadX = 0.0f;
    if (targetFacing == Direction::Right) lookAheadX = m_lookAheadDistance;
    else if (targetFacing == Direction::Left) lookAheadX = -m_lookAheadDistance;

    Vector2 desired = { targetPos.x + lookAheadX, targetPos.y - 12.0f };

    // Smooth lerp
    float t = std::clamp(m_smoothSpeed * dt, 0.0f, 1.0f);
    m_camera.target.x += (desired.x - m_camera.target.x) * t;
    m_camera.target.y += (desired.y - m_camera.target.y) * t;

    // Clamp camera within level bounds
    float halfW = m_viewportWidth * 0.5f;
    float halfH = m_viewportHeight * 0.5f;

    if (m_maxX - m_minX >= m_viewportWidth) {
        m_camera.target.x = std::clamp(m_camera.target.x, m_minX + halfW, m_maxX - halfW);
    } else {
        m_camera.target.x = (m_minX + m_maxX) * 0.5f;
    }

    if (m_maxY - m_minY >= m_viewportHeight) {
        m_camera.target.y = std::clamp(m_camera.target.y, m_minY + halfH, m_maxY - halfH);
    } else {
        m_camera.target.y = (m_minY + m_maxY) * 0.5f;
    }
}

Camera2D CameraController::GetRaylibCamera() const {
    Camera2D cam = m_camera;
    Vector2 shake = ParticleSystem::Instance().GetScreenShakeOffset();
    cam.target.x += shake.x;
    cam.target.y += shake.y;
    return cam;
}

} // namespace riftbound
