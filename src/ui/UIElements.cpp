#include "ui/UIElements.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace riftbound {

UIButton::UIButton(Rectangle bounds, std::string text)
    : m_bounds(bounds),
      m_text(std::move(text)) {
}

void UIButton::Update(Vector2 mousePos) {
    m_isHovered = CheckCollisionPointRec(mousePos, m_bounds);
    if (m_isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_isPressed = true;
    } else {
        m_isPressed = false;
    }
}

bool UIButton::IsClicked() const {
    return m_isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void UIButton::Draw() const {
    Color bg = Color{ 35, 30, 48, 230 };
    Color border = Color{ 80, 85, 115, 255 };
    Color textCol = WHITE;

    if (m_isHovered || m_isSelected) {
        bg = Color{ 60, 50, 85, 255 };
        border = Color{ 255, 170, 40, 255 };
        textCol = Color{ 255, 220, 100, 255 };
    }

    if (m_isPressed) {
        bg = Color{ 90, 70, 120, 255 };
    }

    DrawRectangleRec(m_bounds, bg);
    DrawRectangleLinesEx(m_bounds, (m_isHovered || m_isSelected) ? 2.0f : 1.0f, border);

    int fontSize = 14;
    int textW = MeasureText(m_text.c_str(), fontSize);
    int textX = static_cast<int>(m_bounds.x + (m_bounds.width - textW) * 0.5f);
    int textY = static_cast<int>(m_bounds.y + (m_bounds.height - fontSize) * 0.5f);
    DrawText(m_text.c_str(), textX, textY, fontSize, textCol);
}

UISlider::UISlider(Rectangle bounds, std::string label, float minVal, float maxVal, float initialVal)
    : m_bounds(bounds),
      m_label(std::move(label)),
      m_minVal(minVal),
      m_maxVal(maxVal),
      m_currentVal(initialVal) {
}

void UISlider::SetValue(float val) {
    m_currentVal = std::clamp(val, m_minVal, m_maxVal);
}

void UISlider::Update(Vector2 mousePos) {
    if (CheckCollisionPointRec(mousePos, m_bounds) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_isDragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_isDragging = false;
    }

    if (m_isDragging) {
        float relX = (mousePos.x - m_bounds.x) / m_bounds.width;
        relX = std::clamp(relX, 0.0f, 1.0f);
        m_currentVal = m_minVal + relX * (m_maxVal - m_minVal);
    }
}

void UISlider::Draw() const {
    DrawText(m_label.c_str(), static_cast<int>(m_bounds.x), static_cast<int>(m_bounds.y - 14.0f), 12, LIGHTGRAY);

    // Track
    DrawRectangleRec(m_bounds, Color{ 40, 40, 55, 255 });
    DrawRectangleLinesEx(m_bounds, 1.0f, Color{ 80, 85, 110, 255 });

    // Filled portion
    float pct = (m_currentVal - m_minVal) / (m_maxVal - m_minVal);
    Rectangle fill = { m_bounds.x, m_bounds.y, m_bounds.width * pct, m_bounds.height };
    DrawRectangleRec(fill, Color{ 255, 160, 30, 255 });

    // Handle
    float handleX = m_bounds.x + m_bounds.width * pct;
    DrawRectangle(static_cast<int>(handleX - 4.0f), static_cast<int>(m_bounds.y - 2.0f), 8, static_cast<int>(m_bounds.height + 4.0f), WHITE);
}

} // namespace riftbound
