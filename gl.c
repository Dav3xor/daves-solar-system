//#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include "game.h"

extern Game game;


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

void gl_error(void)
{
  GLenum errCode;
  const GLubyte *errString;

  if ((errCode = glGetError()) != GL_NO_ERROR) {
    errString = gluErrorString(errCode);
    fprintf (stderr, "OpenGL Error: %s\n", errString);
  }
}

GLFWwindow *gl_init(int argc,char *argv[]) {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit ()) {
    fprintf (stderr, "ERROR: could not start GLFW3\n");
    return 1;
  } 

  // uncomment these lines if on Apple OS X
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *vidmode = glfwGetVideoMode(monitor); 
  GLFWwindow *window = glfwCreateWindow(vidmode->width, vidmode->height, "Hello World", monitor, NULL);
  if (!window) {
    fprintf (stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent (window);
                                  

  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable (GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

  /* OTHER STUFF GOES HERE NEXT */


float points[] = {
   0.0f,  0.5f,
   0.5f, -0.5f,
  -0.5f, -0.5f
};

game.gl.vbo = 0;
glGenBuffers (1, &game.gl.vbo);
glBindBuffer (GL_ARRAY_BUFFER, game.gl.vbo);
glBufferData (GL_ARRAY_BUFFER, 6 * sizeof (float), points, GL_STATIC_DRAW);

game.gl.vao = 0;
glGenVertexArrays (1, &game.gl.vao);
glBindVertexArray (game.gl.vao);
glEnableVertexAttribArray (0);
glBindBuffer (GL_ARRAY_BUFFER, game.gl.vbo);
glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

return window;
}



void gl_buildshaders(void)
{


  const char* vertex_shader =
  "#version 400\n"
  "in vec2 vp;"
  "uniform vec2 origin;"
  "uniform vec2 scale;"
  "void main () {"
  "  gl_Position = vec4 ((origin.x-vp.x)*scale.x,"
  "                      (origin.y-vp.y)*scale.y,"
  "                      0.0,"
  "                      1.0);"
  "}";

  const char* fragment_shader =
  "#version 400\n"
  "out vec4 frag_colour;"
  "void main () {"
  "  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
  "}";

  GLuint vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 1, &vertex_shader, NULL);
  glCompileShader (vs);
  gl_printlog(vs);

  GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 1, &fragment_shader, NULL);
  glCompileShader (fs);
  gl_printlog(fs);

  GLuint shape_program = glCreateProgram ();
  glAttachShader (shape_program, fs);
  glAttachShader (shape_program, vs);
  glLinkProgram (shape_program);

  game.gl.shape.program      = shape_program;
  game.gl.shape.origin_loc   = glGetUniformLocation(shape_program, "origin"); 
  game.gl.shape.scale_loc   = glGetUniformLocation(shape_program, "scale"); 
}

void game_loop(GLFWwindow *window)
{
  while (!glfwWindowShouldClose (window)) {
    // wipe the drawing surface clear
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (game.gl.shape.program);

    game.gl.shape.originx = 0.5;    
    game.gl.shape.originy = 0.0;    
    game.gl.shape.scalex  = 0.5;
    game.gl.shape.scaley  = 0.5;

    glUniform2f(game.gl.shape.origin_loc,
                game.gl.shape.originx,
                game.gl.shape.originy);
    
    glUniform2f(game.gl.shape.scale_loc,
                game.gl.shape.scalex,
                game.gl.shape.scaley);

    glBindVertexArray (game.gl.vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3);
    // update other events like input handling 
    glfwPollEvents ();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers (window);
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      break;
    }
  }
  // close GL context and any other GLFW resources
  glfwTerminate();
}







  
