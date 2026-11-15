#include "core/InputManager.hpp"

namespace riftbound {

InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

void InputManager::Update() {
    // Raylib handles polling internally per frame
}

bool InputManager::IsDown(GameAction action) const {
    switch (action) {
        case GameAction::MoveLeft:
            return IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        case GameAction::MoveRight:
            return IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        case GameAction::MoveUp:
            return IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        case GameAction::MoveDown:
            return IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        case GameAction::Jump:
            return IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        case GameAction::MeleeAttack:
            return IsKeyDown(KEY_J) || IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        case GameAction::RangedAttack:
            return IsKeyDown(KEY_K) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        case GameAction::SwitchWeapon:
            return IsKeyDown(KEY_Q);
        case GameAction::Pause:
            return IsKeyDown(KEY_ESCAPE) || IsKeyDown(KEY_P);
        case GameAction::Interact:
            return IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_E);
        case GameAction::ToggleDebug:
            return IsKeyDown(KEY_F1);
        default:
            return false;
    }
}

bool InputManager::IsPressed(GameAction action) const {
    switch (action) {
        case GameAction::MoveLeft:
            return IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT);
        case GameAction::MoveRight:
            return IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT);
        case GameAction::MoveUp:
            return IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
        case GameAction::MoveDown:
            return IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN);
        case GameAction::Jump:
            return IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
        case GameAction::MeleeAttack:
            return IsKeyPressed(KEY_J) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        case GameAction::RangedAttack:
            return IsKeyPressed(KEY_K) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        case GameAction::SwitchWeapon:
            return IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO);
        case GameAction::Pause:
            return IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P);
        case GameAction::Interact:
            return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E);
        case GameAction::ToggleDebug:
            return IsKeyPressed(KEY_F1);
        default:
            return false;
    }
}

bool InputManager::IsReleased(GameAction action) const {
    switch (action) {
        case GameAction::Jump:
            return IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_W) || IsKeyReleased(KEY_UP);
        case GameAction::MeleeAttack:
            return IsKeyReleased(KEY_J) || IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        case GameAction::RangedAttack:
            return IsKeyReleased(KEY_K) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
        default:
            return false;
    }
}

float InputManager::GetHorizontalAxis() const {
    float axis = 0.0f;
    if (IsDown(GameAction::MoveRight)) axis += 1.0f;
    if (IsDown(GameAction::MoveLeft))  axis -= 1.0f;
    return axis;
}

float InputManager::GetVerticalAxis() const {
    float axis = 0.0f;
    if (IsDown(GameAction::MoveDown)) axis += 1.0f;
    if (IsDown(GameAction::MoveUp))   axis -= 1.0f;
    return axis;
}

Vector2 InputManager::GetMousePosition() const {
    return ::GetMousePosition();
}

} // namespace riftbound
