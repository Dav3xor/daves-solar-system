#include "game.h"

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


Shape *poly_ship(void)
{
  Shape *shape = calloc(1, sizeof(Shape));
  shape->numpoints = 4;
  shape->vertices = calloc(shape->numpoints, sizeof(Vertex));
  
  shape->vertices[0].location.x = 0.0;
  shape->vertices[0].location.y = 2.5;
  
  shape->vertices[1].location.x = 1.5;
  shape->vertices[1].location.y = -2.5;
  
  shape->vertices[2].location.x = 0.0;
  shape->vertices[2].location.y = -1.5;
  
  shape->vertices[3].location.x = -1.5;
  shape->vertices[3].location.y = -2.5;
  return shape;
}

Shape *poly_asteroid(unsigned int seed) 
{
  Point a[100];
  Point b[100];

  srand(seed);

  
  unsigned int numsides = 5 + (rand() % 5); 
  printf("numsides = %d\n",numsides); 
  // put a regular polygon into a
  poly_regular(numsides, 50.0, a);

  // bumpify the regular polygon
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);
  numsides = poly_bumpify(b,a,numsides);
  numsides = poly_bumpify(a,b,numsides);

  Shape *shape = calloc(1, sizeof(Shape));
  shape->numpoints = numsides;
  shape->vertices = calloc(numsides, sizeof(Vertex));
  for(int i = 0; i < shape->numpoints; i++) {
    shape->vertices[i].location.x = b[i].x;
    shape->vertices[i].location.y = b[i].y;
    /*
    shape->vertices[i].color[0] = 64;
    shape->vertices[i].color[1] = 192;
    shape->vertices[i].color[2] = 192;
    shape->vertices[i].color[3] = 255;
    */
  }
  return shape;
}






