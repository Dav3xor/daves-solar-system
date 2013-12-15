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
  for(int i=0; i<1000; i++){
    double angle = (rand()%628)/100.0;
    double distance = 300 + (rand()%10000)/50.0;
    double speed = .15 + (rand()%1000)/10000.0;
    if(rand()%2) {
      angle += 3.14159;
    }
    game.ships[i].position.x = sin(angle) * distance;
    game.ships[i].position.y = cos(angle) * distance;
    
    game.ships[i].velocity.i = sin(angle+(3.14159/2.0))*speed;
    game.ships[i].velocity.j = cos(angle+(3.14159/2.0))*speed;
    
    game.ships[i].mass = .00002 + (rand()%100)/1000000.0;
    addpoint(&game.qtree, 
             game.ships[i].position.x,
             game.ships[i].position.y,
             &game.ships[i]);
  }
  
  game.planet[0].position.x = 0.0;
  game.planet[0].position.y = 0.0;
  game.planet[0].velocity.i = 0.0;
  game.planet[0].velocity.j = 0.0;
  game.planet[0].mass = 10.0;
  
  game.planet[1].position.x = 150.0;
  game.planet[1].position.y = 0.0;
  game.planet[1].velocity.i = 0.0;
  game.planet[1].velocity.j = -.255;
  game.planet[1].mass = .10;
  
  game.planet[2].position.x = 155.0;
  game.planet[2].position.y = 0.0;
  game.planet[2].velocity.i = 0.0;
  game.planet[2].velocity.j = -0.176;
  game.planet[2].mass = .001;
  
  game.planet[3].position.x = -10000.0;
  game.planet[3].position.y = 0.0;
  game.planet[3].velocity.i = 0.0;
  game.planet[3].velocity.j = 0.0;
  game.planet[3].mass = 0.0001;
  
  game.asteroid = poly_asteroid(6);  
  
  
  
  gl_init(argc, argv);
}
