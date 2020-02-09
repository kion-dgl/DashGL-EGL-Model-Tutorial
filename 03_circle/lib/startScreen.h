#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

void InitGraphics();
void updateScreen();
void setViewport();
static const char *eglGetErrorStr();

GLuint dgl_create_shader(const char *filename, GLenum type);
GLuint dgl_create_program(const char *vertex, const char *fragment);
void dgl_print_log(GLuint object);
