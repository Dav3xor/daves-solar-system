#include "game.h"
extern Game game;

void poly_regular(unsigned int numsides, double distance, Point *points)
{
  for(int i = 0; i < numsides; i++) {
    points[i].x = sin( i * (TAU/numsides) )*distance;
    points[i].y = cos( i * (TAU/numsides) )*distance;
  }
}

unsigned int poly_bumpify(Point *a, Point *b, unsigned int numsides)
{
  int i,j;

  for(i = 0, j = 0; i < numsides; i++, j++) {
    b[j].x = a[i].x; 
    b[j].y = a[i].y; 

    if(!(rand()%2)) {
      Point *pa = &a[i];
      Point *pb = &a[(i+1) % numsides];

      double dx = pa->x - pb->x;
      double dy = pa->y - pb->y;

      double angle = atan2(dy, dx);

      double cx = (pa->x+pb->x)/2.0;
      double cy = (pa->y+pb->y)/2.0;

      double length = sqrt((dx*dx)+(dy*dy)) / (5.0+((rand()%1000)/100.0));
      
      j++;
      if (rand()%2) {
        angle += TAU/4.0;
      } else {
        angle -= TAU/4.0;
      }

      b[j].x = cx + cos(angle) * length;
      b[j].y = cy + sin(angle) * length;

    }
  }
  return j; 
}

GameObject *make_object(Point *points, unsigned int numpoints)
{
  if ((game.numvertices+numpoints > MAX_VERTICES)||(game.numobjects+1 > MAX_OBJECTS)){
    printf("ERROR: out of vertices\n");
    return NULL;
  }
  GameObject *gobject = &game.gameobjects[game.numobjects];
  gobject->object = &game.objects[game.numobjects];

  Shape *shape = &gobject->shape;  
  shape->startindex = game.numvertices;
  shape->numpoints = numpoints;
  shape->vertices = &game.vertices[game.numvertices];

  for (int i=0; i<numpoints; i++){
    game.vertices[game.numvertices].location.x = points[i].x;
    game.vertices[game.numvertices].location.y = points[i].y;
    game.vertices[game.numvertices].object     = game.numobjects;
    game.numvertices++;
  }
  game.objects[game.numobjects].position.x = 0;
  game.objects[game.numobjects].position.y = 0;
  game.objects[game.numobjects].orientation = 0;

  game.numobjects++;
  printf("%p\n",gobject);
  return gobject;
}

GameObject *poly_ship(void)
{
  Point points[4];
  
  points[0].x = 0.0;
  points[0].y = 2.5;
  
  points[1].x = 1.5;
  points[1].y = -2.5;
  
  points[2].x = 0.0;
  points[2].y = -1.5;
  
  points[3].x = -1.5;
  points[3].y = -2.5;
 
  return make_object(points,4);
}

GameObject *poly_triangle(double size)
{
  Point points[3];

  poly_regular(3, size, points);
  
  return make_object(points,3);
}

GameObject *poly_asteroid(unsigned int seed) 
{
  Point a[100];
  Point b[100];

  srand(seed);

  
  unsigned int numsides = 5 + (rand() % 5); 
  printf("numsides = %d\n",numsides); 
  
  // put a regular polygon into a
  poly_regular(numsides, .2, a);

  // bumpify the regular polygon
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);

  printf("numsides2 = %d\n",numsides);
  
  GameObject *gobject = make_object(b,numsides);
  double angle = (rand()%628)/100.0;
  double distance = 300 + (rand()%10000)/50.0;
  double speed = .15 + (rand()%1000)/10000.0;
  if(rand()%2) {
    angle += 3.14159;
  }
  gobject->object->position.x = sin(angle) * distance;
  gobject->object->position.y = cos(angle) * distance;
  
  gobject->velocity.i = sin(angle+(3.14159/2.0))*speed;
  gobject->velocity.j = cos(angle+(3.14159/2.0))*speed;
  
  gobject->mass = .00002 + (rand()%100)/1000000.0;
  addpoint(&game.qtree, 
           gobject->object->position.x,
           gobject->object->position.y,
           gobject);
  printf("6\n");
  return gobject;
}






