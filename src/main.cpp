#include "game/game.hpp"

int main() {
    SetTraceLogLevel(LOG_DEBUG);

    Game game;
    game.run();
}
