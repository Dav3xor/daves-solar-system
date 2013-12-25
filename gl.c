//#include <GLUT/glut.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//#include <OpenGL/glu.h>
#include <math.h>

#include "game.h"

extern Game game;


void gl_error(void)
{
  GLenum errCode;
  const GLubyte *errString;

  if ((errCode = glGetError()) != GL_NO_ERROR) {
    errString = gluErrorString(errCode);
    fprintf (stderr, "OpenGL Error: %s\n", errString);
  }
}

void gl_printlog(GLuint obj)
{
  int infologLength = 0;
  int maxLength;
 
  if(glIsShader(obj))
    glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
  else
    glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
 
  char infoLog[maxLength];
 
  if (glIsShader(obj))
    glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
  else
    glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
 
  if (infologLength > 0)
    printf("%s\n",infoLog);
}


void gl_buildshaders(Game *game)
{
  const char *shape_vertex_source = 
    "uniform vec2 scale;                                              \n"
    "uniform vec2 origin;                                             \n" 
    "void main()                                                      \n"
    "{                                                                \n"
    "  // Transforming The Vertex                                     \n"
    "  gl_Position.x = (origin.x-gl_Vertex.x)*scale.x;                \n"
    "  gl_Position.y = (origin.y-gl_Vertex.y)*scale.y;                \n"
    "}                                                                \n";
  
  const char *shape_fragment_source = 
    "void main()                                                      \n"
    "{                                                                \n"
    "  // Setting Each Pixel To Red                                   \n"
    "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);                       \n"
    "}                                                                \n";

  GLuint shape_program;
  GLuint shape_vertex_shader;
  GLuint shape_fragment_shader;

  shape_vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shape_vertex_shader, 1, &shape_vertex_source, NULL);
  glCompileShader(shape_vertex_shader);
  gl_printlog(shape_vertex_shader);
  
  shape_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shape_fragment_shader, 1, &shape_fragment_source, NULL);
  glCompileShader(shape_fragment_shader);
  gl_printlog(shape_fragment_shader);

  shape_program = glCreateProgram();
  glAttachShader(shape_program, shape_vertex_shader); 
  glAttachShader(shape_program, shape_fragment_shader); 
  
  glLinkProgram(shape_program);
  
  glUseProgram(shape_program);


  game->gl.shape.scale_loc = glGetUniformLocation(shape_program, "scale"); 
  game->gl.shape.origin_loc = glGetUniformLocation(shape_program, "origin"); 
  printf("-- %d --\n",game->gl.shape.origin_loc);
}

void gl_drawprimitiveship(GameObject *ship)
{
  double px = ship->object.position.x;
  double py = ship->object.position.y;
  glColor3f(.8f, .8f, .8f); // Blue
  glVertex2f((px)-1.f, (py)-1.25f);
  glVertex2f((px)+1.f, (py)-1.25f);
  glVertex2f((px),       (py)+1.25f);
}

void gl_drawprimitiveplanet(GameObject *planet)
{
  glColor3f(1.0f, 1.0f, .5f); // bright pale yellow
  glVertex2f((planet->object.position.x)-2.0f, (planet->object.position.y)-2.5f);
  glVertex2f((planet->object.position.x)+2.0f, (planet->object.position.y)-2.5f);
  glVertex2f((planet->object.position.x),       (planet->object.position.y)+2.5f);
}

void gl_drawshape(Shape *shape)
{
  glVertexPointer(2, GL_DOUBLE, sizeof(Vertex), &shape->vertices[0].location.x);
  glDrawArrays(GL_LINE_LOOP,0,shape->numpoints);
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
    float oldx = game.asteroids[i].object.position.x;
    float oldy = game.asteroids[i].object.position.y;
    
    // do all asteroid-asteroid gravity here
    maptonearby(&game.qtree,
                &handle_gravity,
                &game.asteroids[i],
                oldx, oldy,
                15.0);
     
    do_gravity(&game.asteroids[i],&game.planet[0]);
    do_gravity(&game.asteroids[i],&game.planet[1]);
    do_gravity(&game.asteroids[i],&game.planet[2]);
    do_gravity(&game.asteroids[i],&game.planet[3]);
    
    do_move(&game.asteroids[i]);
    movepoint(&game.qtree,
              oldx,oldy,
              game.asteroids[i].object.position.x,
              game.asteroids[i].object.position.y,
              &game.asteroids[i]);
    
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
  

  for(int i=0; i<1000; i++) {
    gl_drawprimitiveship(&game.asteroids[i]);
  }
  
  gl_drawprimitiveplanet(&game.planet[0]);
  gl_drawprimitiveplanet(&game.planet[1]);
  gl_drawprimitiveplanet(&game.planet[2]);
  gl_drawprimitiveplanet(&game.planet[3]);
  
  glEnd();

  
  counter++;
  if(!(counter%49)){
    free(game.asteroid->vertices);
    free(game.asteroid);
    game.asteroid = poly_asteroid(counter);
  }
  game.asteroid->rotation += 5;

  game.gl.shape.originx = game.planet[1].object.position.x;
  game.gl.shape.originy = game.planet[1].object.position.y;
  

  glUniform2f(game.gl.shape.scale_loc,
              game.gl.shape.scalex,
              game.gl.shape.scaley);
  
  glUniform2f(game.gl.shape.origin_loc,
              game.gl.shape.originx,
              game.gl.shape.originy);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  gl_drawshape(game.asteroid);
  gl_drawshape(game.ship);

  glFlush();

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
 
  gl_buildshaders(&game);
  
  game.gl.shape.scalex = 1.0/(500.0 * ((float)vidmode->width / (float)vidmode->height));
  game.gl.shape.scaley = 1.0/(500.0);
  
  game.gl.shape.scalex *= 4.0;
  game.gl.shape.scaley *= 4.0;

  game.gl.shape.originx = 0.0;
  game.gl.shape.originy = 100.0;
  return window;
}

void game_loop(GLFWwindow *window)
{
  /* Loop until the user closes the window */
  bool stop = false;
  while ((!stop) &&(!glfwWindowShouldClose(window))) {
    
    // update positions.
    game_tick();

    // draw
    gl_display();

    // do double buffering... 
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

