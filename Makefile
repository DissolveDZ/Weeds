
header:
	gcc -E -ISpriteLight/engine_include SpriteLight/engine_include/SpriteLight.h -o SpriteLight/include/engine.h
	cp SpriteLight/include/engine.h include

dynamic:
	clang src/main.c libSpriteLight.so -Iinclude -L. -lfreetype -lSDL2 -lSDL2_mixer -lm -ggdb -Wall -o engine_test

static:
	clang src/main.c libSpriteLight.o -Iinclude -L. -lfreetype -lSDL2 -lSDL2_mixer -lm -ggdb -Wall -o engine_test
	./engine_test

run: dynamic
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/SpriteLight ./engine_test