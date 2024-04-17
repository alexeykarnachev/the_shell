game:
	g++ \
	-Wall \
	-pedantic \
	-std=c++2a \
	-I./deps/include \
	-o ./build/linux/the_shell \
	./src/main.cpp \
	./src/sprite.cpp \
	./src/camera.cpp \
	./src/grid.cpp \
	./src/renderer.cpp \
	./src/resources.cpp \
	./src/geometry.cpp \
	./src/common.cpp \
	-L./deps/lib/linux -lraylib -lGL -lpthread -ldl
