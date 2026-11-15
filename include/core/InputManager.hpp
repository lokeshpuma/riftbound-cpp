#pragma once

#include <raylib.h>

namespace riftbound {

enum class GameAction {
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Jump,
    MeleeAttack,
    RangedAttack,
    SwitchWeapon,
    Pause,
    Interact,
    ToggleDebug
};

class InputManager {
public:
    static InputManager& Instance();

    void Update();

    bool IsDown(GameAction action) const;
    bool IsPressed(GameAction action) const;
    bool IsReleased(GameAction action) const;

    float GetHorizontalAxis() const;
    float GetVerticalAxis() const;

    Vector2 GetMousePosition() const;

private:
    InputManager() = default;
};

} // namespace riftbound
