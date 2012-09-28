CCPPFLAGS=-Wall -Werror -DOPENGL
CFLAGS=-std=c99
CPPFLAGS=
CC=g++
all: ripple.out
OBJECTS=ripple.o OpenGLHelperFunctions.o
ripple.out: $(OBJECTS) makefile
	$(CC) -o ripple.out $(OBJECTS) -lGL -lSDL2 -lGLEW

ripple.o: ripple.c
	$(CC) -c ripple.c -o ripple.o $(CPPFLAGS) $(CCPPFLAGS)

OpenGLHelperFunctions.o: OpenGLHelperFunctions.c
	$(CC) -c OpenGLHelperFunctions.c -o OpenGLHelperFunctions.o $(CPPFLAGS) $(CCPPFLAGS)

clean:
	rm *.o
