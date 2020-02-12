#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "lib/dashgl.h"

#include "bcm_host.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define LCD 0

typedef struct {
	GLuint program;
	GLuint triangle;
	GLuint uniform_fade;
	GLuint attr_coord; 
	GLuint attr_color;

	float fade;
	float fade_dx;
} GLOBAL_T;

static void init_buffers(GLOBAL_T *state);
static void draw_triangles(GLOBAL_T *state);

int main () {

	int terminate = 0;
	GLOBAL_T state;

	// Clear application state
	memset( &state, 0, sizeof( GLOBAL_T ) );
		
	dgl_init_graphics();
	state.program = dgl_create_program("shaders/vertex.glsl", "shaders/fragment.glsl");
	init_buffers(&state);
	dgl_set_viewport();

	state.fade = 1.0f;
	state.fade_dx = -0.006f;

	while (!terminate) {
		draw_triangles(&state);
		dgl_update_screen();
	}

	return 0;
}

static void init_buffers(GLOBAL_T *state) {
	
	// Bind Triangle

	static const GLfloat triangle_data[] = {
		0.0,  0.8, 1.0, 0.0, 0.0,
		-0.8, -0.8, 0.0, 1.0, 0.0,
		0.8, -0.8, 0.0, 0.0, 1.0
	};
	
	glGenBuffers(1, &state->triangle);
	glBindBuffer(GL_ARRAY_BUFFER, state->triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_STATIC_DRAW);

	// Locate Attributes

	const char *attribute_name = "coord2d";
	state->attr_coord = glGetAttribLocation(state->program, attribute_name);
	if(state->attr_coord == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return;
	}

	attribute_name = "v_color";
	state->attr_color = glGetAttribLocation(state->program, attribute_name);
	if(state->attr_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return;
	}

	const char *uniform_name = "fade";
	state->uniform_fade = glGetUniformLocation(state->program, uniform_name);
	if(state->uniform_fade == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return;
	}

	glEnableVertexAttribArray(state->attr_coord);
	glEnableVertexAttribArray(state->attr_color);

}

static void draw_triangles(GLOBAL_T *state) {
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, state->triangle);
	glUseProgram ( state->program );

    state->fade += state->fade_dx;
    if(state->fade <= 0.0f) {
        state->fade = 0.0f;
        state->fade_dx *= -1.0f;
    } else if(state->fade >= 1.0f) {
        state->fade = 1.0f;
        state->fade_dx *= -1.0f;
    }

    glUniform1f(state->uniform_fade, state->fade);



	glVertexAttribPointer(
		state->attr_coord,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		0
	);

	glVertexAttribPointer(
		state->attr_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		(void*)(sizeof(float) * 2)
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glFlush();
	glFinish();

}


