CC = g++

CFLAGS = -Wall -g

LD = g++

LDFLAGS = 

raytracer: main.o tiny_obj_loader.o
	$(LD) $(LDFLAGS) -o raytracer main.o tiny_obj_loader.o

main.o: main.cc
	$(CC) $(CFLAGS) -c -o main.o main.cc

tiny_obj_loader.o: tiny_obj_loader.cpp
	$(CC) $(CFLAGS) -c -o tiny_obj_loader.o tiny_obj_loader.cpp

clean:
	rm *.o
	