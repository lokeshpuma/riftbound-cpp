#pragma once

#include <raylib.h>
#include <string>

namespace riftbound {

class UIButton {
public:
    UIButton(Rectangle bounds, std::string text);

    void Update(Vector2 mousePos);
    void Draw() const;
    bool IsClicked() const;

    void SetSelected(bool sel) { m_isSelected = sel; }
    bool IsSelected() const { return m_isSelected; }
    Rectangle GetBounds() const { return m_bounds; }

private:
    Rectangle m_bounds;
    std::string m_text;
    bool m_isHovered = false;
    bool m_isPressed = false;
    bool m_isSelected = false;
};

class UISlider {
public:
    UISlider(Rectangle bounds, std::string label, float minVal, float maxVal, float initialVal);

    void Update(Vector2 mousePos);
    void Draw() const;
    float GetValue() const { return m_currentVal; }
    void SetValue(float val);

private:
    Rectangle m_bounds;
    std::string m_label;
    float m_minVal;
    float m_maxVal;
    float m_currentVal;
    bool m_isDragging = false;
};

} // namespace riftbound
