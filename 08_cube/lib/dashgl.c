#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "bcm_host.h"
#include "dashgl.h"

#define check() assert(glGetError() == 0)

uint32_t GScreenWidth;
uint32_t GScreenHeight;
EGLDisplay GDisplay;
EGLSurface GSurface;
EGLContext GContext;

void dgl_init_graphics() {

	bcm_host_init();
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,   // You need this line for depth buffering to work
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	GDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(GDisplay!=EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(GDisplay, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(GDisplay, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	GContext = eglCreateContext(GDisplay, config, EGL_NO_CONTEXT, context_attributes);
	assert(GContext!=EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &GScreenWidth, &GScreenHeight);
	assert( success >= 0 );

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = GScreenWidth;
	dst_rect.height = GScreenHeight;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = GScreenWidth << 16;
	src_rect.height = GScreenHeight << 16;		

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );

	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
		0/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

	nativewindow.element = dispman_element;
	nativewindow.width = GScreenWidth;
	nativewindow.height = GScreenHeight;
	vc_dispmanx_update_submit_sync( dispman_update );

	check();

	GSurface = eglCreateWindowSurface( GDisplay, config, &nativewindow, NULL );
	assert(GSurface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(GDisplay, GSurface, GSurface, GContext);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );

	glViewport ( 0, 0, GScreenWidth, GScreenHeight );
	check();

}

float dgl_get_aspect() {

	return 1.0f * GScreenWidth / GScreenHeight;

}

void dgl_update_screen() {
   eglSwapBuffers(GDisplay,GSurface);
}

void dgl_set_viewport() {
   glViewport ( 0, 0, GScreenWidth, GScreenHeight );
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



GLuint dgl_create_shader(const char *filename, GLenum type) {

	FILE *fp;
	int file_len;
	char *source;

	fp = fopen(filename, "rb");
	if(!fp) {
		fprintf(stderr, "Could not open %s for reading\n", filename);
		return 0;
	}
	
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	source = (char*)malloc(file_len + 1);
	fread(source, file_len, 1, fp);
	fclose(fp);
	source[file_len] = '\0';

	const GLchar *sources[] = {
		#ifdef GL_ES_VERSION_2_0
		"#version 100\n", //OpenGL ES 2.0
		#else
		"#version 120\n", // OpenGL 2.1
		#endif
		source
	};

	GLuint shader = glCreateShader(type);
	//glShaderSource(shader, 2, sources, NULL);
	glShaderSource(shader, 2, sources, NULL);
	glCompileShader(shader);

	free((void*)source);

	GLint compile_ok;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

	printf("Compile okay: %d\n", compile_ok);
	if(compile_ok == GL_FALSE) {
		fprintf(stderr, "%s: ", filename);
		dgl_print_log(shader);
		glDeleteShader(shader);
		return 0;
	}

	printf("Shader value: %d\n", shader);
	return shader;

}

GLuint dgl_create_program(const char *vertex, const char *fragment) {

	GLuint vs = dgl_create_shader(vertex, GL_VERTEX_SHADER);
	GLuint fs = dgl_create_shader(fragment, GL_FRAGMENT_SHADER);
	if(vs == 0 || fs == 0) {
		return 0;
	}
	
	GLint link_ok;
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		fprintf(stderr, "Program Link Error: ");
		dgl_print_log(program);
		return 0;
	}
	
	return program;

}

void dgl_print_log(GLuint object) {

	char *log;
	GLint log_length = 0;

	if(glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if(glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		fprintf(stderr, "Not a shader or program\n");
		return;
	}

	log = (char*)malloc(log_length);

	if(glIsShader(object)) {
		glGetShaderInfoLog(object, log_length, NULL, log);
	} else if(glIsProgram(object)) {
		glGetProgramInfoLog(object, log_length, NULL, log);
	}

	fprintf(stderr, "%s\n", log);
	free(log);

}

void vec3_subtract(vec3 a, vec3 b, vec3 v) {

    vec3 tmp;
    tmp[0] = a[0] - b[0];
    tmp[1] = a[1] - b[1];
    tmp[2] = a[2] - b[2];

    v[0] = tmp[0];
    v[1] = tmp[1];
    v[2] = tmp[2];
}

void vec3_cross_multiply(vec3 a, vec3 b, vec3 v) {

    vec3 tmp;

    tmp[0] = a[1]*b[2] - a[2]*b[1];
    tmp[1] = a[2]*b[0] - a[0]*b[2];
    tmp[2] = a[0]*b[1] - a[1]*b[0];

    v[0] = tmp[0];
    v[1] = tmp[1];
    v[2] = tmp[2];

}

void vec3_normalize(vec3 a, vec3 v) {

    float p;

    p = 0.0f;
    p += a[0] * a[0];
    p += a[1] * a[1];
    p += a[2] * a[2];

    p = 1.0f / (float)sqrt(p);

    v[0] = a[0] * p;
    v[1] = a[1] * p;
    v[2] = a[2] * p;

}



void mat4_identity(mat4 m) {

    m[M_00] = 1.0f;
    m[M_01] = 0.0f;
    m[M_02] = 0.0f;
    m[M_03] = 0.0f;
    m[M_10] = 0.0f;
    m[M_11] = 1.0f;
    m[M_12] = 0.0f;
    m[M_13] = 0.0f;
    m[M_20] = 0.0f;
    m[M_21] = 0.0f;
    m[M_22] = 1.0f;
    m[M_23] = 0.0f;
    m[M_30] = 0.0f;
    m[M_31] = 0.0f;
    m[M_32] = 0.0f;
    m[M_33] = 1.0f;

}

void mat4_copy(mat4 a, mat4 m) {

    m[M_00] = a[M_00];
    m[M_01] = a[M_01];
    m[M_02] = a[M_02];
    m[M_03] = a[M_03];
    m[M_10] = a[M_10];
    m[M_11] = a[M_11];
    m[M_12] = a[M_12];
    m[M_13] = a[M_13];
    m[M_20] = a[M_20];
    m[M_21] = a[M_21];
    m[M_22] = a[M_22];
    m[M_23] = a[M_23];
    m[M_30] = a[M_30];
    m[M_31] = a[M_31];
    m[M_32] = a[M_32];
    m[M_33] = a[M_33];

}

void mat4_translate(vec3 t, mat4 m) {

    m[M_00] = 1.0f;
    m[M_10] = 0.0f;
    m[M_20] = 0.0f;
    m[M_30] = 0.0f;

    m[M_01] = 0.0f;
    m[M_11] = 1.0f;
    m[M_21] = 0.0f;
    m[M_31] = 0.0f;

    m[M_02] = 0.0f;
    m[M_12] = 0.0f;
    m[M_22] = 1.0f;
    m[M_32] = 0.0f;

    m[M_03] = t[0];
    m[M_13] = t[1];
    m[M_23] = t[2];
    m[M_33] = 1.0f;

}

void mat4_rotate_y(float y, mat4 m) {

    m[M_00] = cos(y);
    m[M_01] = 0.0f;
    m[M_02] = sin(y);
    m[M_03] = 0.0f;
    m[M_10] = 0.0f;
    m[M_11] = 1.0f;
    m[M_12] = 0.0f;
    m[M_13] = 0.0f;
    m[M_20] =-sin(y);
    m[M_21] = 0.0f;
    m[M_22] = cos(y);
    m[M_23] = 0.0f;
    m[M_30] = 0.0f;
    m[M_31] = 0.0f;
    m[M_32] = 0.0f;
    m[M_33] = 1.0f;

}



void mat4_rotate_z(float z, mat4 m) {

    m[M_00] = cos(z);
    m[M_01] =-sin(z);
    m[M_02] = 0.0f;
    m[M_03] = 0.0f;
    m[M_10] = sin(z);
    m[M_11] = cos(z);
    m[M_12] = 0.0f;
    m[M_13] = 0.0f;
    m[M_20] = 0.0f;
    m[M_21] = 0.0f;
    m[M_22] = 1.0f;
    m[M_23] = 0.0f;
    m[M_30] = 0.0f;
    m[M_31] = 0.0f;
    m[M_32] = 0.0f;
    m[M_33] = 1.0f;

}

void mat4_multiply(mat4 a, mat4 b, mat4 m) {

    mat4 tmp;

    tmp[M_00] = a[M_00]*b[M_00]+a[M_01]*b[M_10]+a[M_02]*b[M_20]+a[M_03]*b[M_30];
    tmp[M_01] = a[M_00]*b[M_01]+a[M_01]*b[M_11]+a[M_02]*b[M_21]+a[M_03]*b[M_31];
    tmp[M_02] = a[M_00]*b[M_02]+a[M_01]*b[M_12]+a[M_02]*b[M_22]+a[M_03]*b[M_32];
    tmp[M_03] = a[M_00]*b[M_03]+a[M_01]*b[M_13]+a[M_02]*b[M_23]+a[M_03]*b[M_33];

    tmp[M_10] = a[M_10]*b[M_00]+a[M_11]*b[M_10]+a[M_12]*b[M_20]+a[M_13]*b[M_30];
    tmp[M_11] = a[M_10]*b[M_01]+a[M_11]*b[M_11]+a[M_12]*b[M_21]+a[M_13]*b[M_31];
    tmp[M_12] = a[M_10]*b[M_02]+a[M_11]*b[M_12]+a[M_12]*b[M_22]+a[M_13]*b[M_32];
    tmp[M_13] = a[M_10]*b[M_03]+a[M_11]*b[M_13]+a[M_12]*b[M_23]+a[M_13]*b[M_33];

    tmp[M_20] = a[M_20]*b[M_00]+a[M_21]*b[M_10]+a[M_22]*b[M_20]+a[M_23]*b[M_30];
    tmp[M_21] = a[M_20]*b[M_01]+a[M_21]*b[M_11]+a[M_22]*b[M_21]+a[M_23]*b[M_31];
    tmp[M_22] = a[M_20]*b[M_02]+a[M_21]*b[M_12]+a[M_22]*b[M_22]+a[M_23]*b[M_32];
    tmp[M_23] = a[M_20]*b[M_03]+a[M_21]*b[M_13]+a[M_22]*b[M_23]+a[M_23]*b[M_33];

    tmp[M_30] = a[M_30]*b[M_00]+a[M_31]*b[M_10]+a[M_32]*b[M_20]+a[M_33]*b[M_30];
    tmp[M_31] = a[M_30]*b[M_01]+a[M_31]*b[M_11]+a[M_32]*b[M_21]+a[M_33]*b[M_31];
    tmp[M_32] = a[M_30]*b[M_02]+a[M_31]*b[M_12]+a[M_32]*b[M_22]+a[M_33]*b[M_32];
    tmp[M_33] = a[M_30]*b[M_03]+a[M_31]*b[M_13]+a[M_32]*b[M_23]+a[M_33]*b[M_33];

    mat4_copy(tmp, m);

}


void mat4_look_at(vec3 eye, vec3 center, vec3 up, mat4 m) {

    mat4 a;
    vec3 f, s, t;

    vec3_subtract(center, eye, f);
    vec3_normalize(f, f);

    vec3_cross_multiply(f, up, s);
    vec3_normalize(s, s);

    vec3_cross_multiply(s, f, t);

    m[0] = s[0];
    m[1] = t[0];
    m[2] =-f[0];
    m[3] = 0.0f;

    m[4] = s[1];
    m[5] = t[1];
    m[6] =-f[1];
    m[7] = 0.0f;

    m[8] = s[2];
    m[9] = t[2];
    m[10] = -f[2];
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

    eye[0] = -eye[0];
    eye[1] = -eye[1];
    eye[2] = -eye[2];

    mat4_translate(eye, a);
    mat4_multiply(m, a, m);

}

void mat4_perspective(float y_fov, float aspect, float n, float f, mat4 m) {

    float const a = 1.f / (float) tan(y_fov / 2.f);

    m[0] = a / aspect;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = a;
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = -((f + n) / (f - n));
    m[11] = -1.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -((2.0f * f * n) / (f - n));
    m[15] = 0.0f;

}


