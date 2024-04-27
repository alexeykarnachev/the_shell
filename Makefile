game:
	g++ \
	-Wall \
	-pedantic \
	-std=c++2a \
	-I./deps/include \
	-o ./build/linux/the_shell \
	./src/core/sprite.cpp \
	./src/core/renderer.cpp \
	./src/core/resources.cpp \
	./src/core/geometry.cpp \
	./src/game/game.cpp \
	./src/game/input.cpp \
	./src/game/quickbar.cpp \
	./src/game/active_item_ghost.cpp \
	./src/main.cpp \
	-L./deps/lib/linux -lraylib -lGL -lpthread -ldl
