#include "core/Config.hpp"

namespace riftbound {

static GameConfig g_gameConfig;

GameConfig& GetGameConfig() {
    return g_gameConfig;
}

} // namespace riftbound
