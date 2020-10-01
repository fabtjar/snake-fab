game:
	mkdir -p build
	clang source/*.c -o build/snake-fab -Wall -Wextra -lSDL2 -lSDL2_image
