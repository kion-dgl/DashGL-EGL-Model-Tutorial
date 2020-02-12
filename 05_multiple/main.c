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
	GLuint square;
	GLuint circle;
	GLuint hexagon;
	GLuint attr_coord; 
	GLuint attr_color; 
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

	while (!terminate) {
		draw_triangles(&state);
		dgl_update_screen();
	}

	return 0;
}

static void init_buffers(GLOBAL_T *state) {
	
	// Bind Triangle

	static const GLfloat triangle_data[] = {
		-0.5,  1.0, 0.0, 0.0, 1.0,
		-1.0, -0.0, 0.0, 0.0, 1.0,
		0.0, -0.0, 0.0, 0.0, 1.0
	};
	
	glGenBuffers(1, &state->triangle);
	glBindBuffer(GL_ARRAY_BUFFER, state->triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_STATIC_DRAW);

	// Bind Square

	static const GLfloat square_data[] = {
		1.0,  1.0, 1.0, 1.0, 0.0,
		1.0,  0.0, 1.0, 1.0, 0.0,
		0.0,  0.0, 1.0, 1.0, 0.0,

		1.0,  1.0, 1.0, 1.0, 0.0,
		0.0,  0.0, 1.0, 1.0, 0.0,
		0.0,  1.0, 1.0, 1.0, 0.0
	};
	
	glGenBuffers(1, &state->square);
	glBindBuffer(GL_ARRAY_BUFFER, state->square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_data), square_data, GL_STATIC_DRAW);
	
	// Bind Circle

	int i;
	const int num_segments = 10;
	float a, b;
	GLfloat *circle_data;
	circle_data = malloc(num_segments * sizeof(GLfloat) * 15);
	memset( circle_data, 0, sizeof( circle_data) );

	for(i = 0; i < num_segments; i++) {

		a = i * 2.0f * M_PI / num_segments;
		b = (i+1) * 2.0f * M_PI / num_segments;

		// A

		circle_data[i*15+0] = cos(a)*0.5 + 0.5;
		circle_data[i*15+1] = sin(a)*0.5 - 0.5;
		circle_data[i*15+2] = 1.0f;
		circle_data[i*15+3] = 0.0f;
		circle_data[i*15+4] = 0.0f;
		
		// B

		circle_data[i*15+5] = 0.5;
		circle_data[i*15+6] = -0.5;
		circle_data[i*15+7] = 1.0f;
		circle_data[i*15+8] = 0.0f;
		circle_data[i*15+9] = 0.0f;

		// C

		circle_data[i*15+10] = cos(b)*0.5 + 0.5;
		circle_data[i*15+11] = sin(b)*0.5 - 0.5;
		circle_data[i*15+12] = 1.0f;
		circle_data[i*15+13] = 0.0f;
		circle_data[i*15+14] = 0.0f;

		printf("Segment: %d\n", i);
	}
	
	glGenBuffers(1, &state->circle);
	glBindBuffer(GL_ARRAY_BUFFER, state->circle);
	glBufferData(GL_ARRAY_BUFFER, num_segments * sizeof(GLfloat) * 15, circle_data, GL_STATIC_DRAW);

	// Bind Hexagon
	
	for(i = 0; i < 6; i++) {

		a = i * 2.0f * M_PI / 6;
		b = (i+1) * 2.0f * M_PI / 6;

		// A

		circle_data[i*15+0] = cos(a)*0.5 - 0.5;
		circle_data[i*15+1] = sin(a)*0.5 - 0.5;
		circle_data[i*15+2] = 0.0f;
		circle_data[i*15+3] = 1.0f;
		circle_data[i*15+4] = 0.0f;
		
		// B

		circle_data[i*15+5] = -0.5;
		circle_data[i*15+6] = -0.5;
		circle_data[i*15+7] = 0.0f;
		circle_data[i*15+8] = 1.0f;
		circle_data[i*15+9] = 0.0f;

		// C

		circle_data[i*15+10] = cos(b)*0.5 - 0.5;
		circle_data[i*15+11] = sin(b)*0.5 - 0.5;
		circle_data[i*15+12] = 0.0f;
		circle_data[i*15+13] = 1.0f;
		circle_data[i*15+14] = 0.0f;

		printf("Segment: %d\n", i);
	}
	
	glGenBuffers(1, &state->hexagon);
	glBindBuffer(GL_ARRAY_BUFFER, state->hexagon);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat) * 15, circle_data, GL_STATIC_DRAW);


	free(circle_data);

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

	glEnableVertexAttribArray(state->attr_coord);
	glEnableVertexAttribArray(state->attr_color);

}

static void draw_triangles(GLOBAL_T *state) {
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, state->triangle);
	glUseProgram ( state->program );

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
	
	glBindBuffer(GL_ARRAY_BUFFER, state->square);
	glUseProgram ( state->program );

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

    glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindBuffer(GL_ARRAY_BUFFER, state->circle);
	glUseProgram ( state->program );

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

    glDrawArrays(GL_TRIANGLES, 0, 3 * 10);
	
	glBindBuffer(GL_ARRAY_BUFFER, state->hexagon);
	glUseProgram ( state->program );

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

    glDrawArrays(GL_TRIANGLES, 0, 3 * 6);

	glFlush();
	glFinish();

}


