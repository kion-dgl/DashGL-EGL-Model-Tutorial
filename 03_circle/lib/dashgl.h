#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

void dgl_init_graphics();
void dgl_update_screen();
void dgl_set_viewport();
static const char *eglGetErrorStr();

GLuint dgl_create_shader(const char *filename, GLenum type);
GLuint dgl_create_program(const char *vertex, const char *fragment);
void dgl_print_log(GLuint object);
