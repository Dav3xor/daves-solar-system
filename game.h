#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../pyquadtree/quadtree.h"

#define PI 3.14159
#define TAU 2*PI

#define MAX_OBJECTS 100
#define MAX_VERTICES 1000

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
typedef struct Object     Object;
typedef struct Game       Game;

struct Point
{
  double x;
  double y;
};

struct Vertex
{
  Point location;
  unsigned int object;
};


struct Shape
{
  double        rotation;
  unsigned int  startindex;
  unsigned int  numpoints;
  Vertex       *vertices;
};


struct Vector
{
  double i;
  double j;
};

struct Object
{
Point   position;
double  orientation;
};

struct GameObject
{
  Object        object;
  Shape         shape;
  Vector        velocity;
  double        mass;
  unsigned int  flags;
};


struct Game
{
  QuadTree     qtree;
  GameObject   asteroids[1000];
  GameObject   planet[4];
  Shape       *asteroid;
  Shape       *ship;
  Vertex       vertices[MAX_VERTICES];
  unsigned int numvertices;
  Object       objects[MAX_OBJECTS];
  unsigned int numobjects;
  struct gl
  {
    GLuint vao;
    GLuint vbo;
    
    struct shape
    {
      GLuint  program;
      GLuint  scale_loc;
      GLuint  origin_loc;
      GLuint  position_loc;
      GLuint  object_loc;
      GLfloat scalex;
      GLfloat scaley;
      GLfloat originx;
      GLfloat originy;
    }shape;
  }gl;
};

void do_gravity(GameObject *a, GameObject *b);
void do_gravity_once(GameObject *a, GameObject *b);
void do_move(GameObject *a);

void print_location(GameObject *a);

void gl_display(void);
void gl_setvertices(void);
GLFWwindow *gl_init(int argc, char *argv[]);
void game_loop(GLFWwindow *window);

void   poly_regular(unsigned int numsides, double distance, Point *points);
Shape *poly_asteroid(unsigned int seed);
Shape *poly_ship(void);



#define TIMING(a)                               \
  clock_t start=clock();                        \
  a;                                            \
  clock_t diff = clock() - start;               \
  clock_t msec = diff * 1000 / CLOCKS_PER_SEC;  \
  //printf("msecs: %d\n",msec); 
