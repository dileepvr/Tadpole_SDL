INCLUDE = -I/usr/include/

#LIBDIR  = -L/usr/X11R6/lib 
LIBDIR = -L/usr/lib
#LIBDIR = -L/usr/lib32/

COMPILERFLAGS = -Wall 

CC = g++
GCC = gcc
GFLAG = -D__LINUX__
#GFLAG = -DWIN32
#GFLAG = -D__APPLE__
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
	$(GCC) -c ipaddr.c $(GFLAG)
check: tadpole
	./tadpole

clean:
	rm -f *.o $(EXECUTABLE)


