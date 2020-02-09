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
	
	int i, k;
	const int num_segments = 10;
	float a, b;

	GLfloat *circle_data;
	circle_data = malloc(num_segments * sizeof(GLfloat) * 9);
	memset( circle_data, 0, sizeof( circle_data) );

	for(i = 0; i < num_segments; i++) {
		a = i * 2.0f * M_PI / num_segments;
		b = (i+1) * 2.0f * M_PI / num_segments;

		circle_data[i*9+0] = cos(a);
		circle_data[i*9+1] = sin(a);
		circle_data[i*9+2] = 1.0f;

		circle_data[i*9+3] = 0;
		circle_data[i*9+4] = 0;
		circle_data[i*9+5] = 1.0f;

		circle_data[i*9+6] = cos(b);
		circle_data[i*9+7] = sin(b);
		circle_data[i*9+8] = 1.0f;

		printf("%.02f, %.02f, %.02f, \n", circle_data[i*9+0], circle_data[i*9+1], circle_data[i*9+2]);
		printf("%.02f, %.02f, %.02f, \n", circle_data[i*9+3], circle_data[i*9+4], circle_data[i*9+5]);
		printf("%.02f, %.02f, %.02f, \n", circle_data[i*9+6], circle_data[i*9+7], circle_data[i*9+8]);

	}
	

	/*
	static const GLfloat vertex_data[] = {
		  -0.8, -0.8, 1.0,
		   0.8, -0.8, 1.0,
		   0.8,  0.8, 1.0,

		  -0.8, -0.8, 1.0,
		   0.8,  0.8, 1.0,
		  -0.8,  0.8, 1.0
	};
	*/

	static const GLfloat vertex_data[] = {
		   1.0,  0.0, 1.0,
		   0.0,  0.0, 1.0,
		   0.81,  0.59, 1.0,

		   0.81, 0.59, 1.0,
		   0.0,  0.0, 1.0,
		  0.31,  0.95, 1.0
	};
	
	printf("Size: %d\n", sizeof(circle_data));

	state->attr_vertex = glGetAttribLocation(state->program, "vertex");
	glGenBuffers(1, &state->buf);

	// Prepare viewport
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
		  
	// Upload vertex data to a buffer
	glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	glBufferData(GL_ARRAY_BUFFER, num_segments * sizeof(GLfloat) * 9, circle_data, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(state->attr_vertex, 3, GL_FLOAT, 0, 12, 0);
	glEnableVertexAttribArray(state->attr_vertex);
	free(circle_data);

}

static void draw_triangles(GLOBAL_T *state) {
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	glUseProgram ( state->program );
	glDrawArrays ( GL_TRIANGLES, 0, 3 * 10 );

	glFlush();
	glFinish();

}


