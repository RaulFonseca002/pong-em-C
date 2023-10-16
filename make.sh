#!/bin/sh

if [ "$1" = "build" ]; then
	echo "Compilando programa"
	gcc -o pong pong.c `sdl2-config --cflags --libs` -lSDL2_image

elif [ "$1" = "run" ]; then
	echo "Inicializando programa"
	./pong
    
else
	echo "Compilando programa"
	gcc -o pong pong.c `sdl2-config --cflags --libs` -lSDL2_image
	echo "Inicializando programa"
	./pong
fi
