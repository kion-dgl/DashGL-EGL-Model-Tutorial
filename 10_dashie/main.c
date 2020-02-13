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
	GLuint attribute_texcoord;
	GLuint uniform_mytexture;
	GLint texture_id;
	float fade;
	float fade_dx;
	unsigned int rot;
} GLOBAL_T;

int tri_count;
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
	
    float *shark_vertices;
    uint32_t i, file_size;
    FILE *fp = fopen("assets/dashie.bin", "rb");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    shark_vertices = malloc(file_size);
    fread(shark_vertices, 1, file_size, fp);
    fclose(fp);

	tri_count = file_size / 4 / 5;

    for(i = 0; i < 20; i++) {
        printf("%d) %0.2f\n", i, shark_vertices[i]);
    }

	printf("Triangle Count: %d\n", tri_count);

    glGenBuffers(1, &state->vbo_cube_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_cube_vertices);
    glBufferData(GL_ARRAY_BUFFER, file_size, shark_vertices, GL_STATIC_DRAW);
    free(shark_vertices);

    state->texture_id = dgl_texture_load("assets/dashie.png");

    const char* attribute_name;
    attribute_name = "coord3d";
    state->attribute_coord3d = glGetAttribLocation(state->program, attribute_name);
    if (state->attribute_coord3d == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        exit(EXIT_FAILURE);
    }

    attribute_name = "texcoord";
    state->attribute_texcoord = glGetAttribLocation(state->program, attribute_name);
    if (state->attribute_texcoord == -1) {
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

    uniform_name = "mytexture";
    state->uniform_mytexture = glGetUniformLocation(state->program, uniform_name);
    if (state->uniform_mytexture == -1) {
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
    glBindBuffer(GL_ARRAY_BUFFER, state->buf);
    glUseProgram ( state->program );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->texture_id);
    glUniform1i(state->uniform_mytexture, 0);

    time_t current_time;
    current_time = time(NULL);
    float rad = state->rot * M_PI / 180.0;
    state->rot++;

    if(state->rot == 360) {
        state->rot = 0;
    }

    vec3 eye = { 0.0f, 2.0f, 0.0f };
    vec3 target = { 0.0f, 1.4f, -4.0f };
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

    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_cube_vertices);
    glEnableVertexAttribArray(state->attribute_coord3d);
    glVertexAttribPointer(
        state->attribute_coord3d,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*5,
        0
    );

    glEnableVertexAttribArray(state->attribute_texcoord);
    glVertexAttribPointer(
        state->attribute_texcoord,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*5,
        (void*)(sizeof(float)*3)
    );

    glDrawArrays ( GL_TRIANGLES, 0, 8418 );
    glDisableVertexAttribArray(state->attribute_coord3d);
    glDisableVertexAttribArray(state->attribute_texcoord);

    glFlush();
    glFinish();

}


