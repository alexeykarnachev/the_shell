#include "game.hpp"

int main() {
    SetTraceLogLevel(LOG_DEBUG);

    the_shell::Game game;
    game.run();
}
