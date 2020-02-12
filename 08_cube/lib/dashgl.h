#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

void dgl_init_graphics();
void dgl_update_screen();
void dgl_set_viewport();
static const char *eglGetErrorStr();

GLuint dgl_create_shader(const char *filename, GLenum type);
GLuint dgl_create_program(const char *vertex, const char *fragment);
float dgl_get_aspect();
void dgl_print_log(GLuint object);


    /**********************************************************************/
    /** Typedef                                                          **/
    /**********************************************************************/

    typedef float mat4[16];
    typedef float vec3[3];

    /**********************************************************************/
    /** Constants                                                        **/
    /**********************************************************************/

    #define M_00 0
    #define M_10 1
    #define M_20 2
    #define M_30 3
    #define M_01 4
    #define M_11 5
    #define M_21 6
    #define M_31 7
    #define M_02 8
    #define M_12 9
    #define M_22 10
    #define M_32 11
    #define M_03 12
    #define M_13 13
    #define M_23 14
    #define M_33 15

    /**********************************************************************/
    /** Vector3 Utilities                                                **/
    /**********************************************************************/

    void vec3_subtract(vec3 a, vec3 b, vec3 v);
    void vec3_cross_multiply(vec3 a, vec3 b, vec3 v);
    void vec3_normalize(vec3 a, vec3 v);

    /**********************************************************************/
    /** Matrix Utilities                                                 **/
    /**********************************************************************/

    void mat4_identity(mat4 m);
    void mat4_copy(mat4 a, mat4 m);
    void mat4_translate(vec3 t, mat4 m);
    void mat4_rotate_y(float y, mat4 m);
    void mat4_rotate_z(float z, mat4 m);
    void mat4_multiply(mat4 a, mat4 b, mat4 m);
    void mat4_look_at(vec3 eye, vec3 center, vec3 up, mat4 m);
    void mat4_perspective(float y_fov, float aspect, float n, float f, mat4 m);
