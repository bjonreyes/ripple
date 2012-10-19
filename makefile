CCPPFLAGS=-Wall -Werror #-DOPENGL
CFLAGS=-std=c99
CPPFLAGS=
CC=g++
all: ripple.out
OBJECTS=ripple.o OpenGLHelperFunctions.o
ripple.out: $(OBJECTS) makefile
	$(CC) -o ripple.out $(OBJECTS) -lGL -lSDL2 -lGLEW

ripple.o: ripple.cpp ripple.h CommonDefines.h
	$(CC) -c ripple.cpp -o ripple.o $(CPPFLAGS) $(CCPPFLAGS)

OpenGLHelperFunctions.o: OpenGLHelperFunctions.cpp OpenGLHelperFunctions.h CommonDefines.h
	$(CC) -c OpenGLHelperFunctions.cpp -o OpenGLHelperFunctions.o $(CPPFLAGS) $(CCPPFLAGS)

clean:
	rm -f *.o
