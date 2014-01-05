#include "game.h"

#define TEST_LORENTZ 1

#ifdef TEST_LORENTZ
extern Game game;
#endif

//  lorentz:
//  return (1-((speed**2)/(5**2)))**(-.5)

// appr logistic:
// 1/(1+math.e**(-((v*8.0)-10.0)))
void do_gravity(GameObject *a, GameObject *b)
{ 

  double dx       = a->object->position.x - b->object->position.x;
  double dy       = a->object->position.y - b->object->position.y;

  double angle    = atan2(dy,dx);
  double distance = sqrt((dx*dx) + (dy*dy));
  double effect   = 1.0/(distance*distance) ;
 
  double ga       = effect * b->mass; 
  double gb       = effect * a->mass; 
 
  a->velocity.i -= cos(angle) * ga;
  a->velocity.j -= sin(angle) * ga;
  
  b->velocity.i += cos(angle) * gb;
  b->velocity.j += sin(angle) * gb;


}


double do_transition(const double current, const double commanded)
{
  return current + ((commanded-current)/32.0);
}

static inline double vector_magnitude(Vector *v)
{
  return sqrt((v->i*v->i)+(v->j*v->j));
}

void do_gravity_once(GameObject *a, GameObject *b)
{
  if (a == b) {
    return;
  }

  double dx       = a->object->position.x - b->object->position.x;
  double dy       = a->object->position.y - b->object->position.y;

  double angle    = atan2(dy,dx);
  double distance = sqrt((dx*dx) + (dy*dy));
  double effect   = 1.0/(distance*distance) ;
 
  double ga       = effect * b->mass; 
  double gb       = effect * a->mass; 

  a->velocity.i -= cos(angle) * ga;
  a->velocity.j -= sin(angle) * ga;
 

}

void do_move(GameObject *a)
{
  #ifdef TEST_LORENTZ
  static float min=10000.0;
  static float max=0.0;
  static float avg=.2;
  #endif

  a->object->position.x += a->velocity.i;
  a->object->position.y += a->velocity.j;
  
  #ifdef TEST_LORENTZ
  double magnitude = vector_magnitude(&a->velocity);
  if (magnitude < min) {
    min = magnitude;
  } else if (magnitude > max) {
    max = magnitude;
  }
  avg = (avg+avg+avg+avg+avg+magnitude)/6.0;
  if (a == &game.asteroids[999]){
    //printf("min = %f max = %f avg = %f\n", min, max, avg);
  }
  #endif
}
