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
	GLuint buf;
	GLuint uniform_mvp;
	GLuint attribute_coord3d;
	GLuint attribute_v_color;
    GLuint vbo_cube_vertices;
	GLuint ibo_cube_elements;
	float fade;
	float fade_dx;
	unsigned int rot;
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
	state.rot = 0;

	while (!terminate) {
		draw_triangles(&state);
		dgl_update_screen();
	}

	return 0;
}

static void init_buffers(GLOBAL_T *state) {
	
	// Bind Triangle

  GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0, 1.0, 0.0, 0.0,
     1.0, -1.0,  1.0, 0.0, 1.0, 0.0,
     1.0,  1.0,  1.0, 0.0, 0.0, 1.0,
    -1.0,  1.0,  1.0, 1.0, 1.0, 1.0,
    -1.0, -1.0, -1.0, 1.0, 0.0, 0.0,
     1.0, -1.0, -1.0, 0.0, 1.0, 0.0,
     1.0,  1.0, -1.0, 0.0, 0.0, 1.0,
    -1.0,  1.0, -1.0, 1.0, 1.0, 1.0
  };
  glGenBuffers(1, &state->vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, state->vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    GLushort cube_elements[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // top
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 0, 3,
        3, 7, 4,
        // left
        4, 5, 1,
        1, 0, 4,
        // right
        3, 2, 6,
        6, 7, 3,
    };
    glGenBuffers(1, &state->ibo_cube_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->ibo_cube_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	/*
    state->program = dash_create_program("shaders/vertex.glsl", "shaders/fragment.glsl");
    if(!state->program) {
        exit(EXIT_FAILURE);
    }
	*/

    const char* attribute_name;
    attribute_name = "coord3d";
    state->attribute_coord3d = glGetAttribLocation(state->program, attribute_name);
    if (state->attribute_coord3d == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        exit(EXIT_FAILURE);
    }

    attribute_name = "v_color";
    state->attribute_v_color = glGetAttribLocation(state->program, attribute_name);
    if (state->attribute_v_color == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        exit(EXIT_FAILURE);
    }

    const char* uniform_name;
    uniform_name = "mvp";
    state->uniform_mvp = glGetUniformLocation(state->program, uniform_name);
    if (state->uniform_mvp == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        exit(EXIT_FAILURE);
    }

glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glDepthMask(GL_TRUE);
glClearDepthf(1.0f);
glDepthRangef(0.1f, 1.0f);


}

static void draw_triangles(GLOBAL_T *state) {
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	glUseProgram ( state->program );

    float rad = state->rot * M_PI / 180.0;
    state->rot++;

    if(state->rot == 360) {
        state->rot = 0;
    }

    vec3 eye = { 0.0f, 2.0f, 0.0f };
    vec3 target = { 0.0f, 0.0f, -4.0f };
    vec3 axis = { 0.0f, 1.0f, 0.0f };
    vec3 t = { 0.0, 0.0, -4.0f };

    mat4 mvp, pos, rot, projection, view;
    mat4_identity(mvp);
	float aspect = dgl_get_aspect();
    mat4_perspective(45.0f, aspect, 0.1f, 10.0f, projection);
    mat4_look_at(eye, target, axis, view);
    mat4_translate(t, pos);
    mat4_rotate_y(rad, rot);

    mat4_multiply(mvp, projection, mvp);
    mat4_multiply(mvp, view, mvp);
    mat4_multiply(mvp, pos, mvp);
    mat4_multiply(mvp, rot, mvp);

    glUniformMatrix4fv(state->uniform_mvp, 1, GL_FALSE, mvp);
    glEnableVertexAttribArray(state->attribute_coord3d);

    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_cube_vertices);
    glVertexAttribPointer(
        state->attribute_coord3d,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*6,
        0
    );

    glEnableVertexAttribArray(state->attribute_v_color);
    glVertexAttribPointer(
        state->attribute_v_color,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*6,
        (void*)(sizeof(float)*3)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->ibo_cube_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(state->attribute_coord3d);
    glDisableVertexAttribArray(state->attribute_v_color);


	
	glFlush();
	glFinish();

}


