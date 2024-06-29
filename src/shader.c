/* clang-format off */
#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
/* clang-format on */

/* Function Prototypes */
GLuint compile_shader (GLuint type, const char *src);

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
