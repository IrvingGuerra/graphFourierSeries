server: server.o gfx.o
	g++ gfx.o server.o -o server -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm
server.o: server.cpp gfx.o
	g++ -std=c++11 server.cpp -c -lpthread
gfx.o: gfx.c gfx.h
	gcc gfx.c -c -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm