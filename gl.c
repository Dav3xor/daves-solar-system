//#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <math.h>

#include "game.h"

extern Game game;


void gl_drawprimitiveship(GameObject *ship)
{
  glColor3f(.8f, .8f, .8f); // Blue
  glVertex2f((ship->position.x/500.0)-0.01f, (ship->position.y/500.0)-0.0125f);
  glVertex2f((ship->position.x/500.0)+0.01f, (ship->position.y/500.0)-0.0125f);
  glVertex2f((ship->position.x/500.0),       (ship->position.y/500.0)+0.0125f);
}

void gl_drawprimitiveplanet(GameObject *planet)
{
  glColor3f(1.0f, 1.0f, .5f); // bright pale yellow
  glVertex2f((planet->position.x/500.0)-0.02, (planet->position.y/500.0)-0.025f);
  glVertex2f((planet->position.x/500.0)+0.02, (planet->position.y/500.0)-0.025f);
  glVertex2f((planet->position.x/500.0),       (planet->position.y/500.0)+0.025f);
}

void gl_drawshape(Shape *shape)
{
  glColor3f(1.0f, 1.0f, 1.0f); // bright pale yellow
  for(int i = 0; i < shape->numpoints; i++) {
    glVertex2f(shape->vertices[i].location.x,  shape->vertices[i].location.y);
  }
  glVertex2f(shape->vertices[0].location.x,  shape->vertices[0].location.y);
}
    
void handle_gravity(LeafData *data, void *arg)
{
  GameObject *a = (GameObject *)data->data;
  GameObject *b = (GameObject *)arg;
  do_gravity_once(a,b);
}


void game_tick(void)
{
  for(int i=0; i<1000; i++){
    float oldx = game.ships[i].position.x;
    float oldy = game.ships[i].position.y;
     
    maptonearby(&game.qtree,
                &handle_gravity,
                &game.ships[i],
                oldx, oldy,
                15.0);
     
    do_gravity(&game.ships[i],&game.planet[0]);
    do_gravity(&game.ships[i],&game.planet[1]);
    do_gravity(&game.ships[i],&game.planet[2]);
    do_gravity(&game.ships[i],&game.planet[3]);
    
    /*            
    for(int j=i+1; j<1000; j++){
      do_gravity(&game.ships[i],&game.ships[j]);
    }
    */
    do_move(&game.ships[i]);
    movepoint(&game.qtree,
              oldx,oldy,
              game.ships[i].position.x,
              game.ships[i].position.y,
              &game.ships[i]);
    
  }



  for(int i=0; i<4; i++){
    for(int j=i+1; j<4; j++){
      do_gravity(&game.planet[i],&game.planet[j]);
    }
  }

  do_move(&game.planet[0]);
  do_move(&game.planet[1]);
  do_move(&game.planet[2]);
  do_move(&game.planet[3]);
}


void gl_display(void)
{
  static int counter = 0;
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_TRIANGLES);          // Each set of 3 vertices form a triangle


  //print_location(&game.ships[0]);
  //print_location(&game.planet);
  for(int i=0; i<1000; i++) {
    gl_drawprimitiveship(&game.ships[i]);
  }
  gl_drawprimitiveplanet(&game.planet[0]);
  gl_drawprimitiveplanet(&game.planet[1]);
  gl_drawprimitiveplanet(&game.planet[2]);
  gl_drawprimitiveplanet(&game.planet[3]);
  glEnd();

  glBegin(GL_LINE_LOOP);
  
  counter++;
  if(!(counter%49)){
    free(game.asteroid);
    game.asteroid = poly_asteroid(counter);
  }
  gl_drawshape(game.asteroid);

  glEnd();

  glFlush();

  //glutSwapBuffers();
}

GLFWwindow *gl_init(int argc, char *argv[])
{
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return NULL;
 
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(vidmode->width, vidmode->height, "Hello World", monitor, NULL);
  if (!window)
  {
    glfwTerminate();
    return NULL;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  
  return window;
}

void game_loop(GLFWwindow *window)
{
  /* Loop until the user closes the window */
  bool stop = false;
  while ((!stop) &&(!glfwWindowShouldClose(window))) {
    /* Render here */
    game_tick();
    gl_display();
    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      stop = true;
    }


  }

  glfwTerminate();
}

/*
void gl_init(int argc, char *argv[]) // Called before main loop to set up the program
{
  glutInit(&argc, argv); // Initializes glut

  // Sets up a double buffer with RGBA components and a depth component
  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

  // Sets the window size to 512*512 square pixels
  glutInitWindowSize(512, 512);

  // Sets the window position to the upper left
  glutInitWindowPosition(0, 0);

  // Creates a window using internal glut functionality
  glutCreateWindow("Hello!");
  // passes reshape and display functions to the OpenGL machine for callback
  glutDisplayFunc(gl_display);
  glutIdleFunc(gl_idle);


  glClearColor(0.0, 0.0, 0.0, 0.0);
  //glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH );
  glEnable(GL_POINT_SMOOTH);
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

  // Starts the program.
  glutMainLoop();
}
*/

