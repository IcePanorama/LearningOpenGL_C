/* clang-format off */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"
/* clang-format on */

/* Function Prototypes */
void initialize_program (void);
void main_loop (void);
void clean_up (void);
void handle_user_input (void);
void predraw (void);
void draw (void);
void print_opengl_version_info (void);
void vertex_specification (void);
void create_graphics_pipeline (void);

/* Global Variables */
int g_screen_width = 640;
int g_screen_height = 480;
SDL_Window *g_graphics_application_window;
SDL_GLContext g_opengl_context;
bool g_quitting = false;
GLuint g_vertex_array_object;
GLuint g_vertex_buffer_object;
GLuint g_graphics_pipeline_shader_program;
GLuint g_index_buffer_object;

int
main (void)
{
  initialize_program ();
  vertex_specification ();
  create_graphics_pipeline ();
  main_loop ();
  clean_up ();
  return 0;
}

void
initialize_program (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      puts ("SDL2 could not initialize video subsytem");
      exit (1);
    }

  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK,
                       SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);

  g_graphics_application_window
      = SDL_CreateWindow ("OpenGL Window", 0, 0, g_screen_width,
                          g_screen_height, SDL_WINDOW_OPENGL);

  if (g_graphics_application_window == NULL)
    {
      puts ("Could not create an SDL_Window.");
      exit (1);
    }

  g_opengl_context = SDL_GL_CreateContext (g_graphics_application_window);

  if (g_opengl_context == NULL)
    {
      puts ("Could not create an SDL_Context.");
      exit (1);
    }

  if (glewInit () != GLEW_OK)
    {
      puts ("Could not initialize GLEW.");
      exit (1);
    }

  print_opengl_version_info ();
}

void
main_loop (void)
{
  while (!g_quitting)
    {
      handle_user_input ();
      predraw ();
      draw ();

      SDL_GL_SwapWindow (g_graphics_application_window);
    }
}

void
clean_up (void)
{
  SDL_GL_DeleteContext (g_opengl_context);
  SDL_DestroyWindow (g_graphics_application_window);
  SDL_Quit ();
}

void
handle_user_input (void)
{
  SDL_Event event;
  while (SDL_PollEvent (&event))
    {
      if (event.type == SDL_QUIT)
        {
          puts ("Goodbye.");
          g_quitting = true;
        }
    }
}

void
predraw (void)
{
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);

  glViewport (0, 0, g_screen_width, g_screen_height);
  glClearColor (1.0f, 1.0f, 0.0f, 1.0f);

  glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glUseProgram (g_graphics_pipeline_shader_program);
}

void
draw (void)
{
  glBindVertexArray (g_vertex_array_object);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object);
  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glUseProgram (0);
}

void
print_opengl_version_info (void)
{
  printf ("Vendor: %s\n", glGetString (GL_VENDOR));
  printf ("Renderer: %s\n", glGetString (GL_RENDERER));
  printf ("Version: %s\n", glGetString (GL_VERSION));
  printf ("Shading Language Version: %s\n",
          glGetString (GL_SHADING_LANGUAGE_VERSION));
}

void
vertex_specification (void)
{
  /* clang-format off */
  const GLfloat vertex_data[] =
  { 
      // first tri
    -0.5f, -0.5f, 0.0f, // bot left
     1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, // bot right
     0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, // top left
     0.0f,  0.0f, 1.0f,
      // second tri
     0.5f,  0.5f, 0.0f, // top right
     0.0f,  1.0f, 0.0f,
  };
  const GLuint index_buffer_data[] = 
    {
      2, 0, 1,
      3, 2, 1
    };
  /* clang-format on */

  glCreateVertexArrays (1, &g_vertex_array_object);
  glBindVertexArray (g_vertex_array_object);

  glGenBuffers (1, &g_vertex_buffer_object);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_data), vertex_data,
                GL_STATIC_DRAW);

  /* IBO */
  glGenBuffers (1, &g_index_buffer_object);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, g_index_buffer_object);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (index_buffer_data),
                index_buffer_data, GL_STATIC_DRAW);

  /* Positions */
  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat), NULL);

  /* Colors */
  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat),
                         (GLvoid *)(3 * sizeof (GLfloat)));

  glBindVertexArray (0);
  glDisableVertexAttribArray (0);
  glDisableVertexAttribArray (1);
}

void
create_graphics_pipeline (void)
{
  char *vertex_shader_source
      = load_shader_as_string ("src/shaders/vertex_shader.glsl");
  char *fragment_shader_source
      = load_shader_as_string ("src/shaders/fragment_shader.glsl");

  g_graphics_pipeline_shader_program
      = create_shader_program (vertex_shader_source, fragment_shader_source);
}
