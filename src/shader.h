#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>

GLuint create_shader_program (const char *vertex_shader_source,
                              const char *fragment_shader_source);
char *load_shader_as_string (const char *FILENAME);

#endif
