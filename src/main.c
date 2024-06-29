#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
GLuint create_shader_program (const char *vertex_shader_source,
                              const char *fragment_shader_source);
GLuint compile_shader (GLuint type, const char *src);
char *load_shader_as_string (const char *FILENAME);

/* Global Variables */
int g_screen_width = 640;
int g_screen_height = 480;
SDL_Window *g_graphics_application_window;
SDL_GLContext g_opengl_context;
bool g_quitting = false;
GLuint g_vertex_array_object;
GLuint g_vertex_buffer_object;
GLuint g_vertex_buffer_object2;
GLuint g_graphics_pipeline_shader_program;

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
  glDrawArrays (GL_TRIANGLES, 0, 6);

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
  const GLfloat vertex_positions[] =
  { 
      // first tri
    -0.5f, -0.5f, 0.0f, // bot left
     0.5f, -0.5f, 0.0f, // bot right
    -0.5f,  0.5f, 0.0f, // top left
      // second tri
     0.5f, -0.5f, 0.0f, // bot right
     0.5f,  0.5f, 0.0f, // top right
    -0.5f,  0.5f, 0.0f, // top left
  };
  const GLfloat vertex_colors[] =
  { 
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };
  /* clang-format on */

  glCreateVertexArrays (1, &g_vertex_array_object);
  glBindVertexArray (g_vertex_array_object);

  /* Positions */
  glGenBuffers (1, &g_vertex_buffer_object);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_positions), vertex_positions,
                GL_STATIC_DRAW);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  /* Colors */
  glGenBuffers (1, &g_vertex_buffer_object2);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object2);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_colors), vertex_colors,
                GL_STATIC_DRAW);

  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindVertexArray (0);
  glDisableVertexAttribArray (0);
  glDisableVertexAttribArray (1);
}

GLuint
compile_shader (GLuint type, const char *src)
{
  GLuint shader_object;

  if (type == GL_VERTEX_SHADER)
    {
      shader_object = glCreateShader (GL_VERTEX_SHADER);
    }
  else if (type == GL_FRAGMENT_SHADER)
    {
      shader_object = glCreateShader (GL_FRAGMENT_SHADER);
    }
  else
    {
      printf ("Invalid shader type. Expected %d or %d, got %d",
              GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, type);
      exit (1);
    }

  glShaderSource (shader_object, 1, &src, NULL);
  glCompileShader (shader_object);

  GLint result;
  glGetShaderiv (shader_object, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE)
    {
      GLint length;
      glGetShaderiv (shader_object, GL_INFO_LOG_LENGTH, &length);
      char *error_messages = malloc (length * sizeof (char));
      if (error_messages == NULL)
        {
          printf ("Unable to malloc error message buffer of size %d.\n",
                  length);
          glDeleteShader (shader_object);
          return shader_object;
        }

      glGetShaderInfoLog (shader_object, length, &length, error_messages);

      if (type == GL_VERTEX_SHADER)
        {
          printf ("Error: GL_VERTEX_SHADER failed to compile.\n%s\n",
                  error_messages);
        }
      else if (type == GL_FRAGMENT_SHADER)
        {
          printf ("Error: GL_FRAGMENT_SHADER failed to compile.\n%s\n",
                  error_messages);
        }

      free (error_messages);

      glDeleteShader (shader_object);
    }

  return shader_object;
}

GLuint
create_shader_program (const char *vertex_shader_source,
                       const char *fragment_shader_source)
{
  GLuint program_object = glCreateProgram ();

  GLuint vertex_shader
      = compile_shader (GL_VERTEX_SHADER, vertex_shader_source);
  GLuint fragment_shader
      = compile_shader (GL_FRAGMENT_SHADER, fragment_shader_source);

  glAttachShader (program_object, vertex_shader);
  glAttachShader (program_object, fragment_shader);
  glLinkProgram (program_object);

  glValidateProgram (program_object);

  glDetachShader (program_object, vertex_shader);
  glDetachShader (program_object, fragment_shader);
  glDeleteShader (vertex_shader);
  glDeleteShader (fragment_shader);

  return program_object;
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

char *
load_shader_as_string (const char *FILENAME)
{
  FILE *file_ptr = fopen (FILENAME, "r");
  if (file_ptr == NULL)
    {
      printf ("Could not open shader source file, %s.\n", FILENAME);
      exit (1);
    }

  fseek (file_ptr, 0, SEEK_END);
  size_t file_size = ftell (file_ptr);
  fseek (file_ptr, 0, SEEK_SET);

  char *output = (char *)malloc (file_size + 1);
  if (output == NULL)
    {
      fclose (file_ptr);
      puts (
          "Error: failed to malloc output of load_shader_as_string function.");
      return NULL;
    }

  if ((fread (output, 1, file_size, file_ptr)) != file_size)
    {
      fclose (file_ptr);
      free (output);
      printf ("Error reading file %s.\n", FILENAME);
      return NULL;
    }

  output[file_size] = '\0';
  fclose (file_ptr);
  return output;
}
