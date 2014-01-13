#include <GLFW/glfw3.h>
#include "game.h"

Game game;
/*
  = { 
  {{0,NULL}, {0.0,0.0},   {0,0}, 0, 100.0, 0},
  {{0,NULL}, {50.0,0.0}, {0,1.8}, 0, 10.0,    0} };
*/
int main(int argc, char *argv[])
{
  newtree(&game.qtree,3,7,(struct Extent){-1000,-1000,1000,1000});
  poly_ship();
  for(int i=0; i<40; i++){
    poly_asteroid(i);
  }
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
  //game.asteroid = poly_asteroid(6);  
  //poly_asteroid(7);
  //game.ship = poly_ship();  
  //gl_setvertices();
  
  GLFWwindow * window = gl_init(argc, argv);
  gl_buildshaders(&game);


  game_loop(window);
  printf("hi\n");
}
