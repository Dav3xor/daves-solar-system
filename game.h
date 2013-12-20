#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../pyquadtree/quadtree.h"

#define PI 3.14159
#define TAU 2*PI
/*
information to render each object:
centerx
centery
angle

information per vertex:
x
y
color

information per object:

*/
typedef struct Point      Point;
typedef struct Vertex     Vertex;
typedef struct Shape      Shape;
typedef struct Vector     Vector;
typedef struct GameObject GameObject;
typedef struct Game       Game;

struct Point
{
  double x;
  double y;
};

struct Vertex
{
  Point location;
  unsigned char color[4];
};


struct Shape
{
  double        rotation;
  unsigned int  numpoints;
  Vertex       *vertices;
};


struct Vector
{
  double i;
  double j;
};

struct GameObject
{
  Shape         shape;
  Point         position;
  Vector        velocity;
  double        direction;
  double        mass;
  unsigned int  flags;
};

struct Game
{
  QuadTree qtree;
  GameObject ships[1000];
  GameObject planet[4];
  Shape *asteroid;
};

void do_gravity(GameObject *a, GameObject *b);
void do_gravity_once(GameObject *a, GameObject *b);
void do_move(GameObject *a);

void print_location(GameObject *a);

void gl_display(void);
GLFWwindow *gl_init(int argc, char *argv[]);
void game_loop(GLFWwindow *window);

void   poly_regular(unsigned int numsides, double distance, Point *points);
Shape *poly_asteroid(unsigned int seed);



#define TIMING(a)                               \
  clock_t start=clock();                        \
  a;                                            \
  clock_t diff = clock() - start;               \
  clock_t msec = diff * 1000 / CLOCKS_PER_SEC;  \
  //printf("msecs: %d\n",msec); 
