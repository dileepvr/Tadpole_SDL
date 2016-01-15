INCLUDE = -I/usr/include/

#LIBDIR  = -L/usr/X11R6/lib 
LIBDIR = -L/usr/lib
#LIBDIR = -L/usr/lib32/

COMPILERFLAGS = -Wall 

CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lSDL -lSDL_image -lSDL_ttf -lSDL_net -lSDL_mixer -lm 

TEST_FILES = main.o 
EXECUTABLE = tadpole

#all: $(EXECUTABLE)


$(EXECUTABLE): main.o 
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIBDIR) $(LIBRARIES)\
	main.o 

main.o: main.cpp
	$(CC) -c main.cpp

check: tadpole
	./tadpole

clean:
	rm -f *.o $(EXECUTABLE)


