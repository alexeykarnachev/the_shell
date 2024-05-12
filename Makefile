game:
	g++ \
	-Wall \
	-pedantic \
	-std=c++2a \
	-I./deps/include \
	-o ./build/linux/the_shell \
	./src/core/*.cpp \
	./src/*.cpp \
	-L./deps/lib/linux -lraylib -lGL -lpthread -ldl
