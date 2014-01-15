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

GameObject *make_object(Shape *shapes, Point *points)
{
  GameObject *gobject = &game.gameobjects[game.numobjects];
  gobject->obj_attr = &game.objects[game.numobjects];
  if(game.numobjects+1 > MAX_OBJECTS) {
    printf("ERROR: out of game objects\n");
    return NULL;
  }
  unsigned int startpoint = 0;
  Shape *shape = &shapes[0];
  while(shape) {
    if ((game.numvertices+shape->numpoints > MAX_VERTICES)) {
      printf("ERROR: out of vertices\n");
      return NULL;
    }
    shape->startindex = game.numvertices;
    shape->vertices = &game.vertices[game.numvertices];
    
    for (int i=startpoint; i<startpoint+shape->numpoints; i++){
      game.vertices[game.numvertices].position.x = points[i].x;
      game.vertices[game.numvertices].position.y = points[i].y;
      game.vertices[game.numvertices].obj_index  = game.numobjects;
      game.vertices[game.numvertices].color_index  = game.numobjects%5;
      /*
      printf("(%f,%f) - %d\n",game.vertices[game.numvertices].position.x,
                              game.vertices[game.numvertices].position.y,
                              game.vertices[game.numvertices].obj_index);
      */
      game.numvertices++;
    }
    startpoint += shape->numpoints; 
    shape = shape->next;
  }
  
  gobject->shapes = shapes;
  gobject->position.x = 0;
  gobject->position.y = 0;
  game.objects[game.numobjects].orientation = 0.0;
  game.objects[game.numobjects].x = .1 + ((rand()%1000)*.0008);
  game.objects[game.numobjects].y = .1 + ((rand()%1000)*.0008);
  if(game.numobjects==1){
    game.objects[game.numobjects].x = .5;
  }
  /*
  printf("(%f,%f) o = %f\n",game.objects[game.numobjects].x,
                     game.objects[game.numobjects].y,
                     game.objects[game.numobjects].orientation);
  */
  game.numobjects++;
  return gobject;
}

GameObject *poly_ship(void)
{
  // TODO: replace with static shape array...
  Shape *shapes = malloc(sizeof(Shape)*2);
  shapes[0].flags      = SHAPE_FLAG_LINELOOP;
  shapes[0].numpoints  = 4;
  shapes[0].next       = &shapes[1];
  
  shapes[1].flags      = SHAPE_FLAG_TRIANGLES;
  shapes[1].numpoints  = 3;
  shapes[1].next       = NULL;
  
  Point points[7];
  
  points[0].x = .025;
  points[0].y = 0.0;
  
  points[1].x = -.025;
  points[1].y = .015;
  
  points[2].x = -.015;
  points[2].y = 0.0;
  
  points[3].x = -.025;
  points[3].y = -.015;

  points[4].x = -.030;
  points[4].y = -.006;

  points[5].x = -.030;
  points[5].y =  .006;

  points[6].x = -.050;
  points[6].y = 0.0;
 
  return make_object(shapes, points);
}

GameObject *poly_triangle(double size)
{
  Point points[3];

  poly_regular(3, size, points);
  
  // TODO: replace with static shape array...
  Shape *shape = malloc(sizeof(Shape));
  shape[0].flags      = SHAPE_FLAG_TRIANGLES;
  shape[0].numpoints  = 3;
  shape[0].next       = NULL;
  
  return make_object(shape,points);
}

GameObject *poly_asteroid(unsigned int seed) 
{
  Point a[100];
  Point b[100];

  srand(seed);

  
  unsigned int numsides = 5 + (rand() % 5); 
  
  // put a regular polygon into a
  poly_regular(numsides, .2, a);

  // bumpify the regular polygon
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);

  // TODO: replace with static shape array...
  Shape *shape = malloc(sizeof(Shape));
  shape[0].flags      = SHAPE_FLAG_LINELOOP;
  shape[0].numpoints  = numsides;
  shape[0].next       = NULL;
  
  GameObject *gobject = make_object(shape,b);
  if (gobject) {
    double angle = (rand()%628)/100.0;
    double distance = 300 + (rand()%10000)/50.0;
    double speed = .15 + (rand()%1000)/10000.0;
    if(rand()%2) {
      angle += 3.14159;
    }
    gobject->position.x = sin(angle) * distance;
    gobject->position.y = cos(angle) * distance;
    
    gobject->velocity.i = sin(angle+(3.14159/2.0))*speed;
    gobject->velocity.j = cos(angle+(3.14159/2.0))*speed;
    
    gobject->mass = .00002 + (rand()%100)/1000000.0;
    addpoint(&game.qtree, 
             gobject->position.x,
             gobject->position.y,
             gobject);
  }
  return gobject;
}






