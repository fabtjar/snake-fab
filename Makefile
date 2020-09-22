game:
	mkdir -p build
	clang *.c -o build/snake-fab -Wall -Wextra -lSDL2 -lSDL2_image
