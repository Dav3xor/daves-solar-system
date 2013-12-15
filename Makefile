# This Makefile is for the simple c test program included with pyquadtree.  Useful for
# checking to see if the c code passes through valgrind, etc...

qt: ../pyquadtree/quadtree.c gl.c main.c physics.c util.c polygon.c game.h
	gcc -o sw ../pyquadtree/quadtree.c gl.c main.c physics.c util.c polygon.c -framework OpenGL -framework GLUT -std=c99 -lm
