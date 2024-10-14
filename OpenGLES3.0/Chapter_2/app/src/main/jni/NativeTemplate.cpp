//  OpenGL ES 3.0 Cookcook code
//  Created by Parminder Singh on 8/29/13.
//  Email: parminder_ce@yahoo.co.in
//  Copyright (c) 2013 macbook. All rights reserved.

#include "NativeTemplate.h"
#include <math.h>

GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
GLfloat gTriangleColors[]   = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
GLuint programID;
GLuint positionAttribHandle;
GLuint colorAttribHandle;
GLuint radianAngle;

float degree = 0;
float radian;

static const char vertexShader[] =
        "#version 300 es                                          \n"
        "in vec4        VertexPosition;                           \n"
        "in vec4        VertexColor;                              \n"
        "uniform float  RadianAngle;                              \n"
        "out vec4       TriangleColor;                            \n"

        "void main() {                                            \n"
        "  mat2 rotation = mat2(cos(RadianAngle), sin(RadianAngle), \n"
        "                       -sin(RadianAngle), cos(RadianAngle)); \n"
        "  vec2 position = rotation * VertexPosition.xy;            \n"
        "  gl_Position   = vec4(position, VertexPosition.zw); \n"
        "  TriangleColor = VertexColor;                           \n"
        "}\n";

static const char fragmentShader[] =
        "#version 300 es            \n"
        "precision mediump float;   \n"
        "in vec4 TriangleColor;     \n"
        "out vec4 FragColor;        \n"
        "void main() {              \n"
        "  FragColor = TriangleColor;\n"
        "}";

/*! \fn static void printGLString(const char *name, GLenum s)
 * This function print the error string.
 */
static void printGLString(const char *name, GLenum s) {
    LOGI("GL %s = %s\n", name, (const char *) glGetString(s));
}

/*! \fn static void checkGlError(const char* op)
 * This function is responsible for checking opengles error.
 * call this function after every opengles funciton call.
 */
static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error= glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

/*! \fn GLuint loadAndCompileShader(GLenum shaderType, const char* pSource)
 * This function depending upon the shaderType creates and compile the program.
 * in case of any compilation error it inform the erroe message.
 */
GLuint loadAndCompileShader(GLenum shaderType, const char* sourceCode) {
    GLuint shader = glCreateShader(shaderType);

    if ( shader ) {
        glShaderSource(shader, 1, &sourceCode, NULL);
        glCompileShader(shader);

        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint linkShader(GLuint vertShaderID, GLuint fragShaderID)
{
    if (!vertShaderID || !fragShaderID) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertShaderID);
        glAttachShader(program, fragShaderID);

        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

/*! \fn GLuint compileAndLinkShader(const char* pVertexSource, const char* pFragmentSource)
 * This function accept the source of the vertex and fragment shader and create the program.
 */
GLuint createProgramExec(const char* vertexSource, const char* fragmentSource) {
    GLuint vsID = loadAndCompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fsID = loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    return linkShader(vsID, fsID);
}

/*! \fn void printOpenGLESInfo()
 * This function print the useful information related to OpenGLES.
 * For example: OpenGLES Version, Maker of GPU, API extensions.
 */
void printOpenGLESInfo(){
    printGLString("Version",	GL_VERSION);
    printGLString("Vendor",		GL_VENDOR);
    printGLString("Renderer",	GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    printGLString("GL Shading Language", GL_SHADING_LANGUAGE_VERSION);
}

void SettingAttributeAndUniform() {
    // Use shader program to apply on current primitives
    glUseProgram( programID );

    // Query and send the uniform variable.
    radianAngle          = glGetUniformLocation(programID, "RadianAngle");

    // Query ‘VertexPosition’ from vertex shader
    positionAttribHandle = glGetAttribLocation(programID, "VertexPosition");
    colorAttribHandle    = glGetAttribLocation(programID, "VertexColor");

    // Send data to shader using queried attrib location
    glVertexAttribPointer(positionAttribHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    glVertexAttribPointer(colorAttribHandle, 3, GL_FLOAT, GL_FALSE, 0, gTriangleColors);

    // Enable vertex position attribute
    glEnableVertexAttribArray(positionAttribHandle);
    glEnableVertexAttribArray(colorAttribHandle);
}

void GraphicsInit()
{
    printOpenGLESInfo();
    programID = createProgramExec(vertexShader, fragmentShader);
    if (!programID) {
        LOGE("Could not create program.");
    }
    checkGlError("GraphicsInit");
}

void GraphicsResize( int width, int height )
{
    glViewport(0, 0, width, height);
}

void GraphicsRender()
{
    // Which buffer to clear? – color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Clear color with black color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    SettingAttributeAndUniform();
    radian = degree++/57.2957795;
    glUniform1f(radianAngle, radian);

    // Draw 3 triangle vertices from 0th index
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

#ifdef __ANDROID__
JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_init( JNIEnv *env, jobject obj, jstring FilePath )
{
    setenv( "FILESYSTEM", env->GetStringUTFChars( FilePath, NULL ), 1 );
    GraphicsInit();
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_resize( JNIEnv *env, jobject obj, jint width, jint height)
{
    GraphicsResize( width, height );
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_step(JNIEnv * env, jobject obj)
{
    GraphicsRender();
}
#endif
