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
                                  
  game.origin.x     = 0.5;    
  game.origin.y     = 0.0;    
  game.scale        = 1.0;
  game.commanded_scale = 1.0;
  game.aspect_ratio =  (double)vidmode->height/(double)vidmode->width;
  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  printf ("Renderer: %s\n", renderer);
  printf ("OpenGL version supported %s\n", version);

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable (GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

  /* OTHER STUFF GOES HERE NEXT */


double points[] = {
   0.0f,  0.5f,
   0.5f, -0.5f,
  -0.5f, -0.5f
};

game.gl.vbo = 0;
glGenBuffers (1, &game.gl.vbo);
glBindBuffer (GL_ARRAY_BUFFER, game.gl.vbo);
printf("sizeof g.v = %d\n",sizeof(game.vertices));
for(int i=0; i<game.numvertices; i++){
  printf("(%f,%f)\n",game.vertices[i].location.x, game.vertices[i].location.y);
}



glBufferData (GL_ARRAY_BUFFER, sizeof(game.vertices), &game.vertices[0], GL_STATIC_DRAW);

game.gl.vao = 0;
glGenVertexArrays (1, &game.gl.vao);
glBindVertexArray (game.gl.vao);
glEnableVertexAttribArray (0);
glBindBuffer (GL_ARRAY_BUFFER, game.gl.vbo);
glVertexAttribPointer (0, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (GLubyte*)NULL);

return window;
}



void gl_buildshaders(Game *game)
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

  game->gl.shape.program      = shape_program;
  game->gl.shape.origin_loc   = glGetUniformLocation(shape_program, "origin"); 
  game->gl.shape.scale_loc   = glGetUniformLocation(shape_program, "scale"); 
}
void gl_draw_shapes(const Game *game)
{
  for (int i=0; i<game->numobjects; i++) {
    const GameObject *gobj = &game->gameobjects[i];
    glDrawArrays(GL_LINE_LOOP,gobj->shape.startindex,gobj->shape.numpoints);
  }
}

void gl_setup_shape_shader(GLFWwindow * window)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram (game.gl.shape.program);

  glUniform2f(game.gl.shape.origin_loc,
              game.origin.x,
              game.origin.y);
  
  glUniform2f(game.gl.shape.scale_loc,
              game.scale*game.aspect_ratio,
              game.scale);

  glBindVertexArray (game.gl.vao);
}

bool gl_handle_input(GLFWwindow *window)
{
  glfwPollEvents ();
  // put the stuff we've been drawing onto the display
  glfwSwapBuffers (window);
  
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // close GL context and any other GLFW resources
    glfwTerminate();
    return false;
  }
  return true;
}

void game_loop(GLFWwindow *window)
{
  bool running = true;
  unsigned int counter = 0;
  while (running) {
    counter++;
    if (counter%100==0) {
      game.commanded_scale = .5 + (float)(rand()%1000)/1000.0;
      printf("cs = %f\n",game.commanded_scale);
    }
    game.scale = do_transition(game.scale,game.commanded_scale);

    gl_setup_shape_shader(window);
    gl_draw_shapes(&game);

    if (glfwWindowShouldClose(window)) {
      running = false;
      printf("closing\n");
    } else if (!gl_handle_input(window)) {
      running = false;
      printf("got escape\n");
    }
  }
}





  
