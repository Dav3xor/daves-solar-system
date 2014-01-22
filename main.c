#include <GLFW/glfw3.h>
#include "game.h"

Game game;


int main(int argc, char *argv[])
{
  pqt_newtree(&game.qtree,3,7,(struct pqt_Extent) {-1000,-1000,1000,1000});
  
  game.playership.gobject = poly_ship();
  game.playership.gattr   = &game.ships.objects[0];

  poly_planet(50.0, 10.0,
              (struct Point) {0.0, 0.0},
              (struct Vector) {0.0, 0.0});
  for(int i=0; i<1000; i++){
    poly_asteroid(i);
  }
  //printshapes();

  /* 
  game.planet[0].object->position.x = 0.0;
  game.planet[0].object->position.y = 0.0;
  game.planet[0].velocity.i = 0.0;
  game.planet[0].velocity.j = 0.0;
  game.planet[0].mass = 10.0;
  
  game.planet[1].object->position.x = 150.0;
  game.planet[1].object->position.y = 0.0;
  game.planet[1].velocity.i = 0.0;
  game.planet[1].velocity.j = -.255;
  game.planet[1].mass = .10;
  
  game.planet[2].object->position.x = 155.0;
  game.planet[2].object->position.y = 0.0;
  game.planet[2].velocity.i = 0.0;
  game.planet[2].velocity.j = -0.176;
  game.planet[2].mass = .001;
  
  game.planet[3].object->position.x = -10000.0;
  game.planet[3].object->position.y = 0.0;
  game.planet[3].velocity.i = 0.0;
  game.planet[3].velocity.j = 0.0;
  game.planet[3].mass = 0.0001;
  */ 
  
  GLFWwindow * window = gl_init(argc, argv);
  gl_buildshaders(&game);


  game_loop(window);
  printf("hi\n");
}
