#include "game.h"

#define TEST_LORENTZ 0

#ifdef TEST_LORENTZ
extern Game game;
#endif

//  lorentz:
//  return (1-((speed**2)/(5**2)))**(-.5)

// appr logistic:
// 1/(1+math.e**(-((v*8.0)-10.0)))

void do_thrust(GameObject *go)
{
  go->velocity.i -= cos(go->obj_attr->orientation)*.0001;   
  go->velocity.j -= sin(go->obj_attr->orientation)*.0001;   
}

void do_gravity(GameObject *a, GameObject *b)
{ 

  double dx       = a->position.x - b->position.x;
  double dy       = a->position.y - b->position.y;

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
  return current + ((commanded-current)/12.0);
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

  double dx       = a->position.x - b->position.x;
  double dy       = a->position.y - b->position.y;

  double angle    = atan2(dy,dx);
  double distance = sqrt((dx*dx) + (dy*dy));
  double effect   = 1.0/(distance*distance) ;
 
  double ga       = effect * b->mass; 
  double gb       = effect * a->mass; 

  a->velocity.i -= cos(angle) * ga;
  a->velocity.j -= sin(angle) * ga;
 

}
void do_move_obj(GameObject *a, double newx, double newy)
{
  double oldx = a->position.x;
  double oldy = a->position.y;
  
  pqt_movepoint(&game.qtree,
                oldx,oldy,
                newx, newy,
                a);

  a->position.x = newx;
  a->position.y = newy;

}

void do_move(GameObject *gobject)
{
  #ifdef TEST_LORENTZ
  static float min=10000.0;
  static float max=0.0;
  static float avg=.2;
  #endif

  
  do_move_obj(gobject, 
              gobject->position.x + gobject->velocity.i, 
              gobject->position.y + gobject->velocity.j); 

  gobject->obj_attr->x  = gobject->position.x;  
  gobject->obj_attr->y  = gobject->position.y;  
  gobject->obj_attr->orientation += gobject->rotational_velocity;

  #ifdef TEST_LORENTZ
  double magnitude = vector_magnitude(&gobject->velocity);
  if (magnitude < min) {
    min = magnitude;
  } else if (magnitude > max) {
    max = magnitude;
  }
  avg = (avg+avg+avg+avg+avg+magnitude)/6.0;
  //if (a == &game.asteroids[999]){
    //printf("min = %f max = %f avg = %f\n", min, max, avg);
  //}
  #endif
}
