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
  glBufferData (GL_ARRAY_BUFFER, sizeof(game.vertices), &game.vertices[0], GL_STATIC_DRAW);
  printf("g.v = %d\n",sizeof(game.vertices));

  game.gl.vao = 0;
  glGenVertexArrays (1, &game.gl.vao);
  glBindVertexArray (game.gl.vao);
  glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
  glEnableVertexAttribArray (2);

  glBindBuffer (GL_ARRAY_BUFFER, game.gl.vbo);
  glVertexAttribPointer  (0, 2, GL_DOUBLE,         GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribIPointer (1, 1, GL_UNSIGNED_INT,             sizeof(Vertex), sizeof(Point));
  glVertexAttribIPointer (2, 1, GL_UNSIGNED_INT,             sizeof(Vertex), sizeof(Point)+sizeof(uint32_t));

  return window;
}



void gl_buildshaders(Game *game)
{


  const char* vertex_shader =
  "#version 400\n"
  "layout(location = 0)in vec2 position;\n"
  "layout(location = 1)in uint index;\n"
  "layout(location = 2)in uint color_index;\n"
  "out vec4 vert_color;\n"
  "uniform vec2 origin;\n"
  "uniform vec2 scale;\n"
  "uniform vec3 attributes["MAX_OBJECTS_STR"];\n"
  "uniform vec4 colors["MAX_SHAPE_COLORS_STR"];\n"
  "void main () {\n"
  "  float cos_angle = cos(attributes[index].z);\n"
  "  float sin_angle = sin(attributes[index].z);\n"
  "  float x = (origin.x + (position.x*cos_angle - position.y*sin_angle) + attributes[index].x)*scale.x;\n"
  "  float y = (origin.y + (position.x*sin_angle + position.y*cos_angle) + attributes[index].y)*scale.y;\n"
  "  gl_Position = vec4 (x, y, 0.0, 1.0);\n"
  "  vert_color = colors[color_index];"
  "}\n";

  const char* fragment_shader =
  "#version 400\n"
  "in   vec4 vert_color;\n"
  "out  vec4 frag_color;\n"
  "void main () {\n"
  "  frag_color = vert_color;\n"
  "}\n";
  printf("---\n");
  printf(vertex_shader);
  printf("---\n");
  printf(fragment_shader);
  printf("---\n");
  GLuint vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource  (vs, 1, &vertex_shader, NULL);
  glCompileShader (vs);
  gl_printlog     (vs);

  GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource  (fs, 1, &fragment_shader, NULL);
  glCompileShader (fs);
  gl_printlog     (fs);

  GLuint shape_program = glCreateProgram ();
  glAttachShader (shape_program, fs);
  glAttachShader (shape_program, vs);
  glLinkProgram (shape_program);

  game->gl.shape.program      = shape_program;
  game->gl.shape.origin_loc   = glGetUniformLocation(shape_program, "origin"); 
  game->gl.shape.scale_loc    = glGetUniformLocation(shape_program, "scale"); 
  game->gl.shape.attr_loc     =  glGetUniformLocation(shape_program, "attributes"); 
  game->gl.shape.colors_loc   =  glGetUniformLocation(shape_program, "colors"); 
  printf("origin = %d\n",game->gl.shape.origin_loc);
  printf("scale = %d\n",game->gl.shape.scale_loc);
  printf("attr_loc = %d\n",game->gl.shape.attr_loc);
  printf("colors_loc = %d\n",game->gl.shape.colors_loc);
}
void gl_draw_shapes(const Game *game)
{
  unsigned int shapeindex = 0;
  Shape *curshape = &gobj->shapes[shapeindex];
  for (int i=0; i<game->numobjects; i++) {
    const GameObject *gobj = &game->gameobjects[i];
    for (int j=0; j<gobj->numshapes; j++) {
      printf("i=%d j=%d shapeindex=%d\n",i,j,shapeindex);
      //Shape *curshape = &gobj->shapes[shapeindex];
      printf("1");
      if (!(curshape->flags&SHAPE_FLAG_SKIP)) {
        printf("2");
        if (curshape->flags&SHAPE_FLAG_LINELOOP) {
          printf("3 %d si=%d np=%d\n",shapeindex,curshape->startindex, curshape->numpoints);
          glDrawArrays(GL_LINE_LOOP,curshape->startindex,curshape->numpoints);
        } else if (curshape->flags&SHAPE_FLAG_TRIANGLES) {
          printf("4 si=%d np=%d",curshape->startindex, curshape->numpoints);
          glDrawArrays(GL_TRIANGLES,curshape->startindex,curshape->numpoints);
        }
      }
      printf("\n");
      //shapeindex++;
    }
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
  glUniform3fv(game.gl.shape.attr_loc,
               MAX_OBJECTS,
               (const GLfloat *)&game.objects[0]);
  glUniform4fv(game.gl.shape.colors_loc,
               MAX_SHAPE_COLORS,
               (const GLfloat *)&shape_colors[0][0]);
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
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    game.objects[0].orientation += .05;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    game.objects[0].orientation -= .05;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    do_thrust(&game.gameobjects[0]);
  }
  if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
    game.commanded_scale *= 1.2;
  }
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
    game.commanded_scale /= 1.2;
  }
  return true;
}

void game_loop(GLFWwindow *window)
{
  bool running = true;
  unsigned int counter = 0;
  while (running) {
    counter++;

    do_move(&game.gameobjects[0]);


    game.scale = do_transition(game.scale,game.commanded_scale);
    game.origin.x = do_transition(game.origin.x,game.commanded_origin.x);
    game.origin.y = do_transition(game.origin.y,game.commanded_origin.y);
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





  
