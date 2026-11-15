#include "game/Game.hpp"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

static void EmscriptenLoop() {
    riftbound::Game::Instance().Step();
}
#endif

int main() {
    auto& game = riftbound::Game::Instance();
    if (!game.Init()) {
        return 1;
    }

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(EmscriptenLoop, 0, 1);
#else
    while (!game.ShouldClose()) {
        game.Step();
    }
#endif

    game.Shutdown();
    return 0;
}
