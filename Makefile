all: main

main: main.c
	gcc -o generator main.c -lassimp
