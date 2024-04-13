game:
	g++ \
	-std=c++2a \
	-I./deps/include \
	-o ./build/linux/the_shell \
	./src/main.cpp \
	-L./deps/lib/linux -lraylib -lGL -lpthread -ldl
