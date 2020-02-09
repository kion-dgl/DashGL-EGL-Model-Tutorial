#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "lib/startScreen.h"

#include "bcm_host.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define LCD 0

typedef struct {
	GLuint program;
	GLuint buf;
	GLuint vshader;
	GLuint fshader;
	GLuint attr_vertex; 
} GLOBAL_T;

static void init_shaders(GLOBAL_T *state);
static void draw_triangles(GLOBAL_T *state);

int main () {

	int terminate = 0;
	GLOBAL_T state;

	// Clear application state
	memset( &state, 0, sizeof( GLOBAL_T ) );
		
	InitGraphics();
	state.program = dgl_create_program("shaders/vertex.glsl", "shaders/fragment.glsl");
	init_shaders(&state);
	setViewport();

	while (!terminate) {
		draw_triangles(&state);
		updateScreen();
	}

	return 0;
}

static void init_shaders(GLOBAL_T *state) {

	static const GLfloat vertex_data[] = {
		  -0.8, -0.8, 1.0,
		   0.8, -0.8, 1.0,
		   0.8,  0.8, 1.0,

		  -0.8, -0.8, 1.0,
		   0.8,  0.8, 1.0,
		  -0.8,  0.8, 1.0
	};

	/*
	const GLchar *vshader_source =
		"attribute vec3 vertex;"
		"void main(void) {"
		"	gl_Position = vec4(vertex, 1.0);"
		"}";
		
	const GLchar *fshader_source =
		"void main(void) {"
		"  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);"
		"}";

	state->vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(state->vshader, 1, &vshader_source, 0);
	glCompileShader(state->vshader);

	state->fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(state->fshader, 1, &fshader_source, 0);
	glCompileShader(state->fshader);

	state->program = glCreateProgram();
	glAttachShader(state->program, state->vshader);
	glAttachShader(state->program, state->fshader);
	glLinkProgram(state->program);
	*/

	state->attr_vertex = glGetAttribLocation(state->program, "vertex");
	glGenBuffers(1, &state->buf);

	// Prepare viewport
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
		  
	// Upload vertex data to a buffer
	glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(state->attr_vertex, 3, GL_FLOAT, 0, 12, 0);
	glEnableVertexAttribArray(state->attr_vertex);

}

static void draw_triangles(GLOBAL_T *state) {
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	glUseProgram ( state->program );
	glDrawArrays ( GL_TRIANGLES, 0, 6 );

	glFlush();
	glFinish();

}


