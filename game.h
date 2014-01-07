#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../pyquadtree/quadtree.h"

#define PI 3.14159
#define TAU 2*PI

#define MAX_OBJECTS 1200
#define MAX_VERTICES 5000

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
  double        orientation;
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
  Object        *object;
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
  GameObject   gameobjects[MAX_OBJECTS];
  Object       objects[MAX_OBJECTS];
  unsigned int numobjects;
  double       aspect_ratio;
  double       scale;
  double       commanded_scale;
  Point        origin;
  Point        commanded_origin;

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
    }shape;
  }gl;
};

void do_gravity(GameObject *a, GameObject *b);
double do_transition(const double current, const double commanded);
void do_gravity_once(GameObject *a, GameObject *b);
void do_move(GameObject *a);

void print_location(GameObject *a);

void gl_display(void);
void gl_setvertices(void);
void gl_buildshaders(Game *game);

GLFWwindow *gl_init(int argc, char *argv[]);
void   game_loop(GLFWwindow *window);

void   poly_regular(unsigned int numsides, double distance, Point *points);
Shape *poly_triangle(double size);
Shape *poly_asteroid(unsigned int seed);
Shape *poly_ship(void);



#define TIMING(a)                               \
  clock_t start=clock();                        \
  a;                                            \
  clock_t diff = clock() - start;               \
  clock_t msec = diff * 1000 / CLOCKS_PER_SEC;  \
  //printf("msecs: %d\n",msec); 
