#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "../pyquadtree/quadtree.h"

#define PI 3.14159
#define TAU 2*PI

#define MAX_SHAPES  2500
#define MAX_OBJECTS 5000 
#define MAX_OBJECTS_STR "500"
#define MAX_OBJ_PER_PASS 100 

#define MAX_SHAPE_COLORS 5
#define MAX_SHAPE_COLORS_STR "5"

#define MAX_VERTICES 100000

#define SHAPE_FLAG_SKIP         1
#define SHAPE_FLAG_LINELOOP     2 
#define SHAPE_FLAG_TRIANGLES    4 
#define SHAPE_FLAG_LINE_STRIP   8

typedef struct Point       Point;
typedef struct Vertex      Vertex;
typedef struct Shape       Shape;
typedef struct Vector      Vector;
typedef struct GameObject  GameObject;
typedef struct Object_Attr Object_Attr;
typedef struct DrawList    DrawList;
typedef struct Game        Game;
typedef struct Extent      Extent;
typedef struct PlayerShip  PlayerShip;

struct Point
{
  double x;
  double y;
};

struct Vertex
{
  Point     position;
  uint32_t  obj_index;
  uint32_t  color_index;
};

struct Shape
{
  unsigned int  startindex;
  unsigned int  numpoints;
  unsigned int  flags;
  Vertex       *vertices;
};


struct Vector
{
  double i;
  double j;
};

struct Object_Attr
{
float x;
float y;
float orientation;
} __attribute__ ((packed));

struct GameObject
{
  Object_Attr  *obj_attr;
  Shape         *shapes;
  Point         position;
  Vector        velocity;
  double        rotational_velocity;
  double        mass;
  unsigned int  numshapes;
  unsigned int  flags;
};

struct Extent
{
  unsigned int begin;
  unsigned int end;
};

static float shape_colors[MAX_SHAPE_COLORS][4] = 
{ {1.0, 1.0, 1.0, 1.0},     // white
  { .5,  .5,  .5, 1.0},     // gray
  { .5,  .5, 1.0, 1.0},     // blue
  {1.0,  .5,  .5, 1.0},     // red
  { .5, 1.0,  .5, 1.0} };   // green

struct PlayerShip
{
  GameObject    *gobject;
  Object_Attr   *gattr;
  unsigned int  flags;
  GameObject    *landed_on;
};

struct DrawList
{
  GLuint           vao;
  GLuint           vbo;
  Vertex           vertices[MAX_VERTICES];
  Shape            shapes[MAX_SHAPES];
  GameObject       gameobjects[MAX_OBJECTS];
  Object_Attr      objects[MAX_OBJECTS];
  unsigned int     numvertices;
  unsigned int     numshapes;
  unsigned int     numobjects;
  unsigned int     switchover[100];
};


struct Game
{
  DrawList         ships;
  DrawList         planets;
  DrawList         asteroids;
  pqt_QuadTree     qtree;
  double           aspect_ratio;
  double           scale;
  double           commanded_scale;
  Point            origin;
  Point            commanded_origin;
  PlayerShip       playership;
  struct gl
  {
    
    struct shape
    {
      GLuint  program;
      GLuint  scale_loc;
      GLuint  origin_loc;
      GLuint  attr_loc;
      GLuint  colors_loc;
      GLuint  position_loc;
      GLuint  object_loc;
    }shape;
  }gl;
};

void do_gravity(GameObject *a, GameObject *b);
double do_transition(const double current, const double commanded);
void do_gravity_once(GameObject *a, GameObject *b);
void do_move(GameObject *a);
void do_thrust(GameObject *go);
void do_move_obj(GameObject *a, double newx, double newy);
void print_location(GameObject *a);

void gl_display(void);
void gl_setvertices(void);
void gl_buildshaders(Game *game);

GLFWwindow *gl_init(int argc, char *argv[]);
void   game_loop(GLFWwindow *window);

void   poly_regular(unsigned int numsides, double distance, Point *points);
GameObject *poly_triangle(double size);
GameObject *poly_asteroid(unsigned int seed);
GameObject *poly_ship(void);

GameObject *poly_planet(double size, double mass,
                        Point position, 
                        Vector velocity);


#define TIMING(a)                               \
  clock_t start=clock();                        \
  a;                                            \
  clock_t diff = clock() - start;               \
  clock_t msec = diff * 1000 / CLOCKS_PER_SEC;  \
  //printf("msecs: %d\n",msec); 
