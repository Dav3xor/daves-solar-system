#include "game.h"
extern Game game;


GameObject *make_object(DrawList *dlist, Point *points, Shape *shapes, unsigned int numshapes)
{
  double blah = dlist->numobjects *.2;
  if(dlist->numobjects+1 > MAX_OBJECTS) {
    printf("ERROR: out of game objects\n");
    return NULL;
  }

  if(dlist->numshapes+numshapes > MAX_SHAPES) {
    printf("ERROR: out of shapes\n");
    return NULL;
  }
  
  GameObject *gobject      = &dlist->gameobjects[dlist->numobjects];
  gobject->obj_attr        = &dlist->objects[dlist->numobjects];
   
  
  Shape *startshape        = &dlist->shapes[dlist->numshapes];
  unsigned int startpoint  = 0;
  unsigned int objposition = dlist->numobjects%MAX_OBJ_PER_PASS;

  for(int i=0; i<numshapes; i++) {
    Shape *shape = &shapes[i];
    if ((dlist->numvertices+shape->numpoints > MAX_VERTICES)) {
      printf("ERROR: out of vertices\n");
      return NULL;
    }
    shape->startindex = dlist->numvertices;
    shape->vertices = &dlist->vertices[dlist->numvertices];
    
    for (int i=startpoint; i<startpoint+shape->numpoints; i++){
      dlist->vertices[dlist->numvertices].position.x   = points[i].x;
      dlist->vertices[dlist->numvertices].position.y   = points[i].y;
      dlist->vertices[dlist->numvertices].obj_index    = objposition;
      dlist->vertices[dlist->numvertices].color_index  = dlist->numobjects%5;
      /*
      printf("(%f,%f) - %d\n",game.vertices[game.numvertices].position.x,
                              game.vertices[game.numvertices].position.y,
                              game.vertices[game.numvertices].obj_index);
      */
      dlist->numvertices++;
    }
    dlist->shapes[dlist->numshapes] = *shape;
    dlist->numshapes++;
    startpoint += shape->numpoints; 
  }
  
  gobject->numshapes  = numshapes;
  gobject->shapes     = startshape;
  gobject->position.x = 0;
  gobject->position.y = 0;
  dlist->objects[dlist->numobjects].orientation = 0.0;
  dlist->objects[dlist->numobjects].x = blah;//.1 + ((rand()%1000)*.0008);
  dlist->objects[dlist->numobjects].y = 0.0;//.1 + ((rand()%1000)*.0008);
  /* 
  printf("%.5d (%f,%f) o = %f\n",dlist->numobjects,dlist->objects[dlist->numobjects].x,
                     dlist->objects[dlist->numobjects].y,
                     dlist->objects[dlist->numobjects].orientation);
  */
  dlist->numobjects++;
  
  if((dlist->numobjects) && (dlist->numobjects%MAX_OBJ_PER_PASS == 0)){
    unsigned int position = (dlist->numobjects/MAX_OBJ_PER_PASS) - 1;
    printf("seeting switchover: %d, %d\n",position,dlist->numshapes); 
    dlist->switchover[position] = dlist->numshapes;
  }
  
  pqt_addpoint(&game.qtree, 
           gobject->position.x,
           gobject->position.y,
           gobject);

  return gobject;
}



void poly_regular(unsigned int numsides, double distance, Point *points)
{
  for(int i = 0; i < numsides; i++) {
    points[i].x = cos( i * (TAU/numsides) )*distance;
    points[i].y = sin( i * (TAU/numsides) )*distance;
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

void printshapes(DrawList *dlist)
{
  printf("shapes:\n");
  for(int i=0; i<dlist->numshapes; i++) {
    Shape *shape = &dlist->shapes[i];
    printf("%d %d %d %d %p\n",
           i,
           shape->startindex,
           shape->numpoints,
           shape->flags,
           shape->vertices);
  }
  printf("endshapes:\n");
}
           

GameObject *poly_ship(void)
{
  Shape shapes[2];
  shapes[0].flags      = SHAPE_FLAG_LINELOOP;
  shapes[0].numpoints  = 4;
  
  shapes[1].flags      = SHAPE_FLAG_TRIANGLES|SHAPE_FLAG_SKIP;
  shapes[1].numpoints  = 3;
  
  Point points[7];
 
  double scale = 10;
  points[0].x = .0025*scale;
  points[0].y = 0.0*scale;
  
  points[1].x = -.0025*scale;
  points[1].y = .0015*scale;
  
  points[2].x = -.0015*scale;
  points[2].y = 0.0*scale;
  
  points[3].x = -.0025*scale;
  points[3].y = -.0015*scale;

  points[4].x = -.0030*scale;
  points[4].y = -.0006*scale;

  points[5].x = -.0030*scale;
  points[5].y =  .0006*scale;

  points[6].x = -.0050*scale;
  points[6].y = 0.0*scale;
  
  GameObject *ship = make_object(&game.ships, points, &shapes[0], 2);

  if (ship) {
    do_move_obj (ship, 350.0, 350.0);
  }

  return ship;
}

GameObject *poly_triangle(double size)
{
  Point points[3];

  poly_regular(3, size, points);
  
  Shape shape;
  shape.flags      = SHAPE_FLAG_TRIANGLES;
  shape.numpoints  = 3;
  
  return make_object(&game.asteroids, points,&shape,1);
}

GameObject *poly_planet(double size, double mass,
                        Point position, 
                        Vector velocity)
{
  const double numpoints = 80; 
  Point points[100];
  poly_regular(numpoints, size, points);
  
  Shape shape;
  shape.flags      = SHAPE_FLAG_LINELOOP;
  shape.numpoints  = numpoints;
  
  GameObject *gobject = make_object(&game.planets, points,&shape,1);
  if (gobject) {
    do_move_obj(gobject,
                position.x,
                position.y);
    gobject->velocity = velocity;
    
    gobject->mass = mass;
  }
  return gobject;
}


#define POLAR(angle,distance)           \
  (Point){cos(angle)*distance,          \
          sin(angle)*distance};

#define ADDPOINT(shape,points, point)   \
  points[shape.numpoints] = point;      \
  shape.numpoints += 1;                 \



GameObject *poly_asteroid(unsigned int seed) 
{
  Point a[100];
  Point b[100];

  srand(seed);

  
  unsigned int numsides = 5 + (rand() % 5); 
  double radius = .2;

  // put a regular polygon into a
  poly_regular(numsides, radius, a);

  // bumpify the regular polygon
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);

  Shape shapes[2];
  shapes[0].flags      = SHAPE_FLAG_LINELOOP;
  shapes[0].numpoints  = numsides;
 
  // landing platform
  shapes[1].flags      = SHAPE_FLAG_LINELOOP;
  shapes[1].numpoints  = 0;

  
  double platform_arcwidth = .4 * radius*TAU;
  double platform_height   = radius+.1;
  unsigned int start_point = 0;
  unsigned int end_point = 0; 
  double start_angle = atan2(b[start_point].y, b[start_point].x);
  double end_angle = start_angle+platform_arcwidth;

  Point *base = &b[numsides];
  ADDPOINT(shapes[1], base, POLAR(0,.2));
  ADDPOINT(shapes[1], base, POLAR(TAU/3.0,.2));
  ADDPOINT(shapes[1], base, POLAR(TAU/3.0*2.0,.2));

  ADDPOINT(shapes[1], base, POLAR(0,.2));
  ADDPOINT(shapes[1], base, POLAR(0,.19));

  ADDPOINT(shapes[1], base, POLAR(TAU/3.0,.19));
  ADDPOINT(shapes[1], base, POLAR(TAU/3.0*2.0,.19));
  ADDPOINT(shapes[1], base, POLAR(0,.19));
  /* 
  ADDPOINT(shapes[1], base, POLAR(start_angle,platform_height));

  for(double i = start_angle; i <= end_angle; i += (end_angle-start_angle)/5.0) {
    ADDPOINT(shapes[1],base,POLAR(i,platform_height));
  }

  ADDPOINT(shapes[1], base, POLAR(end_angle,platform_height));
  ADDPOINT(shapes[1], base, POLAR(end_angle,platform_height-.005));
  for(double i = end_angle; i >= start_angle; i -= (end_angle-start_angle)/5.0) {
    ADDPOINT(shapes[1],base,POLAR(i,platform_height-.01));
  }
  ADDPOINT(shapes[1], base, POLAR(start_angle,platform_height-.01));
  ADDPOINT(shapes[1], base, POLAR(start_angle,platform_height-.005));
  ADDPOINT(shapes[1], base, POLAR(start_angle,platform_height));
  */

  GameObject *gobject = make_object(&game.asteroids, b,&shapes[0],2);
  if (gobject) {
    double angle = (rand()%628)/100.0;
    double distance = 300 + (rand()%10000)/50.0;
    double speed = .15 + (rand()%1000)/10000.0;
    if(rand()%2) {
      angle += 3.14159;
    }

    do_move_obj (gobject, 
                 sin(angle) * distance,
                 cos(angle) * distance);
    
    gobject->velocity.i = sin(angle+(3.14159/2.0))*speed;
    gobject->velocity.j = cos(angle+(3.14159/2.0))*speed;
    
    gobject->mass = .000002;
    gobject->rotational_velocity = .005;
  }
  return gobject;
}






