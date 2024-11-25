#include "SimpleTexture.h"
#include "GaussianBlur.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"Vertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"EdgeDetectionFragment.glsl"
#else
#define HORIZONTAL_BLUR_SHADER_PRG	( char * )"shader/BlurHorizontal.glsl"
#define VERTICAL_BLUR_SHADER_PRG	( char * )"shader/BlurVertical.glsl"
#define VERTEX_SHADER_PRG			( char * )"shader/Vertex.glsl"
#define SIMPLE_VERTEX_SHADER_PRG			( char * )"shader/FBOVertex.glsl"
#define SIMPLE_FRAGMENT_SHADER_PRG			( char * )"shader/FBOFragment.glsl"

#endif

#define VERTEX_POSITION 0
#define TEX_COORD 1

// Change the filter size in the BlurHorizontal.glsl and BlurVertical.glsl
#define FILTER_SIZE 5

static float rotationX = 0.0;
static float rotationY = 0.0;
static float lastX = 0;
static float lastY = 0;
#define PACE 1.0
static float deltaX = 0;
static float deltaY = 0;
static float DecelerationFactor = 0.5;

float texCoords[8] ={   0.0f, 0.0f,
                        1.0f, 0.0f,
                        0.0f, 1.0f,
                        1.0f, 1.0f
};

float quad[12]     ={
        -1.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
        -1.0f, 1.0f, -0.0f,
        1.0f, 1.0f, -0.0f
};

extern int TEXTURE_WIDTH;
extern int TEXTURE_HEIGHT;

#define GetUniform ProgramManagerObj->ProgramGetUniformLocation

// Namespace used
using std::ifstream;
using std::ostringstream;

/*!
 Simple Default Constructor

 \param[in] None.
 \return None

 */
#include <queue>
#include <mutex>

std::queue<std::pair<float, float>> touchEvents;
std::mutex touchMutex;
float pixelScale = 0.002;
float scaleFactor = 1.;
float sigma = 100.0;

SimpleTexture::SimpleTexture( Renderer* parent )
{
    if (!parent)
        return;

    RendererHandler	= parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    modelType 			= CubeType;
    glEnable	( GL_DEPTH_TEST );
}


/*!
 Simple Destructor

 \param[in] None.
 \return None

 */
SimpleTexture::~SimpleTexture()
{
    // Remove shaders
    PROGRAM* program = NULL;
    if ( (program = ProgramManagerObj->Program( ( char * )"HorizontalBlur" )) )
    {
        ProgramManagerObj->RemoveProgram(program);
    }

    if ( (program = ProgramManagerObj->Program( ( char * )"VerticalBlur" )) )
    {
        ProgramManagerObj->RemoveProgram(program);
    }
}

/*!
 Initialize the scene, reserve shaders, compile and cache program

 \param[in] None.
 \return None

 */
void SimpleTexture::SimpleInitModel() {
    ProgramManager* ProgramManagerObj   = RendererHandler->RendererProgramManager();
    Transform*	TransformObj            = RendererHandler->RendererTransform();

    if (! ( program = ProgramManagerObj->Program( ( char * )"SimpleShader" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"SimpleShader" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( SIMPLE_VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( SIMPLE_FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);

    //////////////////////////////////////////////////////
    /////////// Vertex shader //////////////////////////
    //////////////////////////////////////////////////////
    CACHE *m = reserveCache( SIMPLE_VERTEX_SHADER_PRG, true );

    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache(m);
    }

    m = reserveCache( SIMPLE_FRAGMENT_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache(m);
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );

    glUseProgram( program->ProgramID );

    MVP = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"MODELVIEWPROJECTIONMATRIX" );
    TEX = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "Tex1" );
    glUniform1i(TEX, 5);
    return;
}


void SimpleTexture::InitHorizontalModel()
{
    // Horizontal Blur Shader
    if (! ( program = ProgramManagerObj->Program( ( char * )"HorizontalBlur" ) ) )
    {
        LOGI("Init HorizontalBlur");
        program = ProgramManagerObj->ProgramInit( ( char * )"HorizontalBlur" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( HORIZONTAL_BLUR_SHADER_PRG, GL_FRAGMENT_SHADER	);

    /////////// Simple Vertex & vertical Blur Fragment shader //////////
    CACHE* m = reserveCache( VERTEX_SHADER_PRG, true );

    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }

    m = reserveCache( HORIZONTAL_BLUR_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );
    glUseProgram( program->ProgramID );

    // Note: the Vertex Shader for horizontal and and vertical gaussian blur is same: Vertex.glsl

    // Get the uniforms from the vertex shader
    MVP                 = GetUniform( program,(char*)"ModelViewProjectionMatrix" );
    TEX_HOR             = GetUniform( program, (char *) "Tex1" );


    // Get the uniforms from the horizontal pass gaussian blur shader
    GAUSSIAN_WEIGHT_HOR = GetUniform(program,(char*) "Weight[0]");
    PIXEL_OFFSET_HOR    = GetUniform(program,(char*) "PixOffset[0]");
    SCREEN_COORD_X_HOR  = GetUniform( program, (char *) "ScreenCoordX" );
    PIXELSIZE           = GetUniform( program, (char *) "pixelSize" );

    glUniform1f(SCREEN_COORD_X_HOR, TEXTURE_WIDTH  / 2.);
    // Set the Pixel size information
    glUniform2f(PIXELSIZE, pixelScale, pixelScale);

    float gWeight[FILTER_SIZE];

    float sum   = 0;
    gWeight[0]  = GaussianBlur::GaussianEquation(0, sigma) * scaleFactor;
    sum         = gWeight[0];// Weight for centered texel

    for(int i = 1; i<FILTER_SIZE; i++){
        gWeight[i] = GaussianBlur::GaussianEquation(i, sigma) * scaleFactor;
        // Why multiplied by 2.0? because each weight is applied in +ve and -ve
        // direction from the centered texel in the fragment shader.
        sum += 2.0 * gWeight[i];
    }

    for(int i = 0; i<FILTER_SIZE; i++){
        gWeight[i] = gWeight[i] / sum;
    }

    if (GAUSSIAN_WEIGHT_HOR >= 0){
        glUniform1fv(GAUSSIAN_WEIGHT_HOR, sizeof(gWeight)/sizeof(float), gWeight);
    }

    float pixOffset[FILTER_SIZE];
    for(int i = 0; i<FILTER_SIZE; i++){
        pixOffset[i] = float(i);
    }

    if (PIXEL_OFFSET_HOR >= 0){
        glUniform1fv(PIXEL_OFFSET_HOR, sizeof(pixOffset)/sizeof(float), pixOffset);
    }

    glUniform1i(TEX_HOR, 0);
    return;
}

void SimpleTexture::InitVerticalModel()
{
    LOGI("Init VerticalBlur");
    float gWeight[FILTER_SIZE];
    float pixOffset[FILTER_SIZE];
    float sum = 0;

    // Vertical Blur Shader
    if (! ( program = ProgramManagerObj->Program( ( char * )"VerticalBlur" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"VerticalBlur" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( VERTICAL_BLUR_SHADER_PRG, GL_FRAGMENT_SHADER	);

    /////////// Simple Vertex & vertical Blur Fragment shader //////////
    CACHE* m = reserveCache( VERTEX_SHADER_PRG, true );
    m = reserveCache( VERTEX_SHADER_PRG, true );

    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }

    m = reserveCache( VERTICAL_BLUR_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );

    glUseProgram( program->ProgramID );

    // Get the uniforms from the horizontal pass gaussian blur shader
    GAUSSIAN_WEIGHT_VERT    = GetUniform(program,(char*)"Weight[0]");
    PIXEL_OFFSET_VERT       = GetUniform(program,(char*)"PixOffset[0]");
    SCREEN_COORD_X_VERT     = GetUniform( program, (char *) "ScreenCoordX" );
    PIXELSIZE               = GetUniform( program, (char *) "pixelSize" );

    // Get the uniforms from the vertex shader
    MVP                  = GetUniform( program,(char*)"ModelViewProjectionMatrix" );
    TEX_VERT             = GetUniform( program, (char *) "Tex1" );


    glUniform1f(SCREEN_COORD_X_VERT, TEXTURE_WIDTH / 2.);
    // Set the Pixel size information
    glUniform2f(PIXELSIZE, pixelScale, pixelScale);

    memset(gWeight, 0, sizeof(float)*FILTER_SIZE);

    sum         = 0;
    gWeight[0]  = GaussianBlur::GaussianEquation(0, sigma) * scaleFactor;
    sum         = gWeight[0]; // Weight for centered texel

    for(int i = 1; i<FILTER_SIZE; i++){
        gWeight[i] = GaussianBlur::GaussianEquation(i, sigma) * scaleFactor;

        // Why multiplied by 2.0? because each weight is applied in +ve and -ve
        // direction from the centered texel in the fragment shader.
        sum += 2.0 * gWeight[i];
    }

    for(int i = 0; i<FILTER_SIZE; i++){
        gWeight[i] = gWeight[i] / sum;
    }

    if (GAUSSIAN_WEIGHT_VERT >= 0){
        glUniform1fv(GAUSSIAN_WEIGHT_VERT, sizeof(gWeight)/sizeof(float), gWeight);
    }

    for(int i = 0; i<FILTER_SIZE; i++){
        pixOffset[i] = float(i);
    }

    if (PIXEL_OFFSET_VERT >= 0){
        glUniform1fv(PIXEL_OFFSET_VERT, sizeof(pixOffset)/sizeof(float), pixOffset);
    }

    glUniform1i(TEX_VERT, 3);
    return;
}
/*!
 Initialize the scene, reserve shaders, compile and cache program

 \param[in] None.
 \return None

*/
void SimpleTexture::SimpleRender()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(program->ProgramID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    GLboolean isCullFaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &isCullFaceEnabled);

    TransformObj->TransformPushMatrix();
    TransformObj->TransformRotate(rotationX, 0.0, 1.0, 0.0);
    TransformObj->TransformRotate(rotationY, 1.0, 0.0, 0.0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1i(TEX, 0);

    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, quad);

    glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
    TransformObj->TransformPopMatrix();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(VERTEX_POSITION);
    glDisableVertexAttribArray(TEX_COORD);
}

void SimpleTexture::Render()
{
    while (!touchEvents.empty()) {
        std::lock_guard<std::mutex> lock(touchMutex);
        auto event = touchEvents.front();
        touchEvents.pop();

        // OpenGL API 호출
        LOGI("Render TOUCH EVENT!!");
        program = ApplyShader(VerticalBlurShader);
        glUseProgram( program->ProgramID );
        SCREEN_COORD_X_VERT  = GetUniform( program, (char *) "ScreenCoordX" );
        if (SCREEN_COORD_X_VERT < 0) LOGI("Error: SCREEN_COORD_X_VERT not found in VerticalBlurShader\n");
        glUniform1f(SCREEN_COORD_X_VERT, event.first);

        program = ApplyShader(HorizontalBlurShader);
        glUseProgram( program->ProgramID );
        SCREEN_COORD_X_HOR  = GetUniform( program, (char *) "ScreenCoordX" );
        if (SCREEN_COORD_X_HOR < 0) LOGI("Error: SCREEN_COORD_X_HOR not found in VerticalBlurShader\n");
        glUniform1f(SCREEN_COORD_X_HOR, event.first);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(program->ProgramID);

    TransformObj->TransformPushMatrix();
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, quad);
    glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
    TransformObj->TransformPopMatrix();

    float retrievedValue;
    glGetUniformfv(program->ProgramID, SCREEN_COORD_X_HOR, &retrievedValue);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SimpleTexture::TouchEventDown( float x, float y )
{
    std::lock_guard<std::mutex> lock(touchMutex);
    touchEvents.push({x, y});

}

void SimpleTexture::TouchEventMove( float x, float y )
{

    lastX = x;
    lastY = y;
}

void SimpleTexture::TouchEventRelease( float x, float y )
{
    deltaX = x - lastX;
    deltaY = y - lastY;
}

PROGRAM* SimpleTexture::ApplyShader(ShaderType shader)
{
    if (shader == HorizontalBlurShader){
        program = ProgramManagerObj->Program( ( char * )"HorizontalBlur" );
    }
    else if (shader == VerticalBlurShader){
        program = ProgramManagerObj->Program( ( char * )"VerticalBlur" );
    }
    else if (shader == SimpleShader) {
        program = ProgramManagerObj->Program( ( char * )"SimpleShader" );
    }
    return program;
}

