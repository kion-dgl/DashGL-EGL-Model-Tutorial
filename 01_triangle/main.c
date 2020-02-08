#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "bcm_host.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define LCD 0

typedef struct {
	uint32_t screen_width;
	uint32_t screen_height;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	GLuint program;
	GLuint buf;
	GLuint vshader;
	GLuint fshader;
	GLuint attr_vertex; 
} GLOBAL_T;

static void init_ogl(GLOBAL_T *state);
static void init_shaders(GLOBAL_T *state);
static void draw_triangles(GLOBAL_T *state);
static const char *eglGetErrorStr();

int main () {

	int terminate = 0;
	bcm_host_init();
	GLOBAL_T state;

	// Clear application state
	memset( &state, 0, sizeof( GLOBAL_T ) );
		
	// Start OGLES
	init_ogl(&state);
	init_shaders(&state);

	while (!terminate) {
		draw_triangles(&state);
	}

	return 0;
}

static void init_ogl(GLOBAL_T *state) {

	int major, minor;
	EGLint num_config;
	static EGL_DISPMANX_WINDOW_T nativewindow;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;
	EGLConfig config;

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
	
	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(state->display == EGL_NO_DISPLAY) {
		fprintf(stderr, "Failed to get EGL display! Error: %s\n", eglGetErrorStr());
		exit(EXIT_FAILURE);
	}

	// initialize the EGL display connection
	if (eglInitialize(state->display, &major, &minor) == EGL_FALSE) {
		fprintf(stderr, "Failed to get EGL version! Error: %s\n", eglGetErrorStr());
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	printf("Initialized EGL version: %d.%d\n", major, minor);

	// get an appropriate EGL frame buffer configuration
	if(eglChooseConfig(state->display, attribute_list, &config, 1, &num_config) == EGL_FALSE) {
		fprintf(stderr, "Failed to get EGL Config! Error: %s\n", eglGetErrorStr());
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	// get an appropriate EGL frame buffer configuration
	if(eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
		fprintf(stderr, "Failed to bind OpenGL API! Error: %s\n", eglGetErrorStr());
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	if(state->context == EGL_NO_CONTEXT) {
		fprintf(stderr, "Failed to get EGL Context! Error: %s\n", eglGetErrorStr());
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	// create an EGL window surface
	if(graphics_get_display_size(LCD, &state->screen_width, &state->screen_height) < 0) {
		fprintf(stderr, "Failed to get display size!\n");
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = state->screen_width;
	dst_rect.height = state->screen_height;
		
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state->screen_width;
	src_rect.height = state->screen_height;

	dispman_display = vc_dispmanx_display_open(LCD);
	dispman_update = vc_dispmanx_update_start( 0 );
			
	dispman_element = vc_dispmanx_element_add ( 
		dispman_update, 
		dispman_display,
		0/*layer*/, 
		&dst_rect, 
		0/*src*/,
		&src_rect, 
		DISPMANX_PROTECTION_NONE, 
		0 /*alpha*/, 
		0/*clamp*/, 
		0/*transform*/
	);
		
	nativewindow.element = dispman_element;
	nativewindow.width = state->screen_width;
	nativewindow.height = state->screen_height;
	vc_dispmanx_update_submit_sync( dispman_update );

	state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
	if(state->surface == EGL_NO_SURFACE) {
		fprintf(stderr, "Failed to create surface! Error: %s\n", eglGetErrorStr());
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}

	// connect the context to the surface
	if(eglMakeCurrent(state->display, state->surface, state->surface, state->context) == EGL_FALSE) {
		fprintf(stderr, "Failed to make current! Error: %s\n", eglGetErrorStr());
		eglDestroySurface(state->display, state->surface);
		eglTerminate(state->display);
		exit(EXIT_FAILURE);
	}
	

}


static void init_shaders(GLOBAL_T *state) {

	static const GLfloat vertex_data[] = {
		  -1.0,-1.0,1.0,
		  1.0,-1.0,1.0,
		  0.0,1.0,1.0
	};

	const GLchar *vshader_source =
		"attribute vec3 vertex;"
		"void main(void) {"
		"	gl_Position = vec4(vertex, 1.0);"
		"}";
		
	const GLchar *fshader_source =
		"void main(void) {"
		"  gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);"
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

	state->attr_vertex = glGetAttribLocation(state->program, "vertex");
	glGenBuffers(1, &state->buf);

	// Prepare viewport
	glViewport ( 0, 0, state->screen_width, state->screen_height );
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
	glDrawArrays ( GL_TRIANGLES, 0, 3 );

	glFlush();
	glFinish();
	eglSwapBuffers(state->display, state->surface);

}

static const char *eglGetErrorStr() {

	switch (eglGetError()) {
	case EGL_SUCCESS:
		return "The last function succeeded without error.";
	case EGL_NOT_INITIALIZED:
		return "EGL is not initialized, or could not be initialized, for the "
			   "specified EGL display connection.";
	case EGL_BAD_ACCESS:
		return "EGL cannot access a requested resource (for example a context "
			   "is bound in another thread).";
	case EGL_BAD_ALLOC:
		return "EGL failed to allocate resources for the requested operation.";
	case EGL_BAD_ATTRIBUTE:
		return "An unrecognized attribute or attribute value was passed in the "
			   "attribute list.";
	case EGL_BAD_CONTEXT:
		return "An EGLContext argument does not name a valid EGL rendering "
			   "context.";
	case EGL_BAD_CONFIG:
		return "An EGLConfig argument does not name a valid EGL frame buffer "
			   "configuration.";
	case EGL_BAD_CURRENT_SURFACE:
		return "The current surface of the calling thread is a window, pixel "
			   "buffer or pixmap that is no longer valid.";
	case EGL_BAD_DISPLAY:
		return "An EGLDisplay argument does not name a valid EGL display "
			   "connection.";
	case EGL_BAD_SURFACE:
		return "An EGLSurface argument does not name a valid surface (window, "
			   "pixel buffer or pixmap) configured for GL rendering.";
	case EGL_BAD_MATCH:
		return "Arguments are inconsistent (for example, a valid context "
			   "requires buffers not supplied by a valid surface).";
	case EGL_BAD_PARAMETER: 
		return "One or more argument values are invalid.";
	case EGL_BAD_NATIVE_PIXMAP:
		return "A NativePixmapType argument does not refer to a valid native "
			   "pixmap.";
	case EGL_BAD_NATIVE_WINDOW:
		return "A NativeWindowType argument does not refer to a valid native "
			   "window.";
	case EGL_CONTEXT_LOST:
		return "A power management event has occurred. The application must "
			   "destroy all contexts and reinitialise OpenGL ES state and "
			   "objects to continue rendering.";
		break;
	}
	
	return "Unknown error!";

}

