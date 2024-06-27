#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdio.h>

void initialize_program (void);
void main_loop (void);
void clean_up (void);
void handle_user_input (void);
void predraw (void);
void draw (void);
void print_opengl_version_info (void);

/* Globals */
int g_screen_height = 480;
int g_screen_width = 640;
SDL_Window *g_graphics_application_window;
SDL_GLContext g_opengl_context;
int g_quitting = 0;

int
main (void)
{
  initialize_program ();
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
      puts ("Could not create an SDL_Window");
      exit (1);
    }

  g_opengl_context = SDL_GL_CreateContext (g_graphics_application_window);

  if (g_opengl_context == NULL)
    {
      puts ("Could not create an SDL_Context.");
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
          g_quitting = 1;
        }
    }
}

void
predraw (void)
{
}

void
draw (void)
{
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
