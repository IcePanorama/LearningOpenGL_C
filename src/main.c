#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

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

/* Globals */
int g_screen_height = 480;
int g_screen_width = 640;
SDL_Window *g_graphics_application_window;
SDL_GLContext g_opengl_context;
bool g_quitting = false;
GLuint g_vertex_array_object;
GLuint g_vertex_buffer_object;
GLuint g_graphics_pipeline_shader_program;
const char *g_VERTEX_SHADER_SOURCE
    = "#version 410 core\n"
      "in vec4 position;\n"
      "void main()\n"
      "{\n"
      " gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
      "}\n";
const char *g_FRAGMENT_SHADER_SOURCE
    = "#version 410 core\n"
      "out vec4 color;\n"
      "void main()\n"
      "{\n"
      " color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
      "}\n";

int
main (void)
{
  initialize_program ();
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
      vertex_specification ();
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

  glUseProgram (g_graphics_pipeline_shader_program);
}

void
draw (void)
{
  glBindVertexArray (g_vertex_array_object);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object);
  glDrawArrays (GL_TRIANGLES, 0, 3);
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
  const GLfloat vertex_positions[]
      = { -0.8f, -0.8f, 0.0f, 0.8f, -0.8f, 0.0f, 0.0f, 0.8f, 0.0f };

  glCreateVertexArrays (1, &g_vertex_array_object);
  glBindVertexArray (g_vertex_array_object);

  glGenBuffers (1, &g_vertex_buffer_object);
  glBindBuffer (GL_ARRAY_BUFFER, g_vertex_buffer_object);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_positions), vertex_positions,
                GL_STATIC_DRAW);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindVertexArray (0);
  glDisableVertexAttribArray (0);
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
  g_graphics_pipeline_shader_program = create_shader_program (
      g_VERTEX_SHADER_SOURCE, g_FRAGMENT_SHADER_SOURCE);
}
