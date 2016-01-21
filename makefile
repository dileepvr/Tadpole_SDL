INCLUDE = -I/usr/include/

#LIBDIR  = -L/usr/X11R6/lib 
LIBDIR = -L/usr/lib
#LIBDIR = -L/usr/lib32/

COMPILERFLAGS = -Wall 

CC = g++
GCC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lSDL -lSDL_image -lSDL_ttf -lSDL_net -lSDL_mixer -lm 

TEST_FILES = main.o 
EXECUTABLE = tadpole

#all: $(EXECUTABLE)


$(EXECUTABLE): main.o ipaddr.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIBDIR) $(LIBRARIES)\
	main.o ipaddr.o

main.o: main.cpp
	$(CC) -c main.cpp

ipaddr.o: ipaddr.c
	$(GCC) -c ipaddr.c
check: tadpole
	./tadpole

clean:
	rm -f *.o $(EXECUTABLE)


