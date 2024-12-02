#include "SimpleTexture.h"
#include "Bloom.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"Vertex.glsl"
#define BLOOM_SHADER_PRG			( char * )"Bloom.glsl"
#define HORIZONTAL_BLUR_SHADER_PRG	( char * )"BlurHorizontal.glsl"
#define VERTICAL_BLUR_SHADER_PRG	( char * )"BlurVertical.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/Vertex.glsl"
#define BLOOM_SHADER_PRG			( char * )"shader/Bloom.glsl"
#define FINAL_BLOOM_SHADER_PRG			( char * )"shader/FinalBloom.glsl"
#define HORIZONTAL_BLUR_SHADER_PRG	( char * )"shader/BlurHorizontal.glsl"
#define VERTICAL_BLUR_SHADER_PRG	( char * )"shader/BlurVertical.glsl"

#define PHONG_FRAGMENT_SHADER_PRG	( char * )"shader/PhongFragment.glsl"
#define PHONG_VERTEX_SHADER_PRG	( char * )"shader/PhongVertex.glsl"
#define SIMPLE_FRAGMENT_SHADER_PRG	( char * )"shader/SimpleFragment.glsl"
#define SIMPLE_VERTEX_SHADER_PRG	( char * )"shader/SimpleVertex.glsl"
#endif

#define VERTEX_POSITION 0
#define TEX_COORD 1

// Change the filter size in the BlurHorizontal.glsl and BlurVertical.glsl
#define FILTER_SIZE 5
#define PIXEL_INTERVAL 2.0
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

float quad[12] = {
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f,
    -1.0f, 1.0f, -0.0f,
     1.0f, 1.0f, -0.0f
};


// 정점 데이터
float quad2[] = {
       -1.0f, -1.0f, 0.0f,  // Vertex 1
        1.0f, -1.0f, 0.0f,  // Vertex 2
       -1.0f, 1.0f, 0.0f,  // Vertex 3
       1.0f,  1.0, 0.0f   // Vertex 4
};

// Namespace used
using std::ifstream;
using std::ostringstream;

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

SimpleTexture::~SimpleTexture()
{
	PROGRAM* program = NULL;
	if ( (program = ProgramManagerObj->Program( ( char * )"square" )) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

void SimpleTexture::InitSimpleModel() {
    LOGI("InitSimpleMode");
    if (! ( program = ProgramManagerObj->Program( ( char * )"SimpleShader" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"SimpleShader" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( SIMPLE_VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( SIMPLE_FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);

    /////////// Simple Vertex & Bloom shader //////////
    CACHE* m = reserveCache( SIMPLE_VERTEX_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }

    m = reserveCache( SIMPLE_FRAGMENT_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );
    glUseProgram( program->ProgramID );

    // VAO 설정
    glGenVertexArrays(1, &SimpleVAO);
    glBindVertexArray(SimpleVAO);
    simpleTEX = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "simpleTexture" );
    glUniform1i(simpleTEX, 0);

    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad2), quad2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glGenBuffers(1, &TextureVbo);
    glBindBuffer(GL_ARRAY_BUFFER, TextureVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SimpleTexture::InitModel()
{
    // Horizontal Blur Shader
    if (! ( program = ProgramManagerObj->Program( ( char * )"HorizontalBlur" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"HorizontalBlur" );
        ProgramManagerObj->AddProgram( program );
    }
    
    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( HORIZONTAL_BLUR_SHADER_PRG, GL_FRAGMENT_SHADER	);
    
    /////////// Simple Vertex & Horizontal Blur Fragment shader //////////
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

    MVP = ProgramManagerObj->ProgramGetUniformLocation( program,(char*)"MODELVIEWPROJECTIONMATRIX" );
    TEX = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "Tex1" );
    GAUSSIAN_WEIGHT_HOR = ProgramManagerObj->ProgramGetUniformLocation(program,(char*)"Weight[0]");
    PIXEL_OFFSET_HOR = ProgramManagerObj->ProgramGetUniformLocation(program,(char*)"PixOffset[0]");
    SCREEN_COORD_X_HOR = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "ScreenCoordX" );
    glUniform1i(TEX, 0);

    float gWeight[FILTER_SIZE];
    float sigma = 10.0;
    float sum   = 0;
    gWeight[0] = Bloom::GaussianEquation(0, sigma);
    sum = gWeight[0];
    for(int i = 1; i<FILTER_SIZE; i++){
        gWeight[i] = Bloom::GaussianEquation(i, sigma);
        sum += 2*gWeight[i];
    }
    
    for(int i = 0; i<FILTER_SIZE; i++){
        gWeight[i] = gWeight[i] / sum;
    }
    
    if (GAUSSIAN_WEIGHT_HOR >= 0){
        glUniform1fv(GAUSSIAN_WEIGHT_HOR, sizeof(gWeight)/sizeof(float), gWeight);
    }
    
    float pixOffset[FILTER_SIZE];
    for(int i = 0; i<FILTER_SIZE; i++){
        pixOffset[i] = float(i)*PIXEL_INTERVAL;
    }
    
    if (PIXEL_OFFSET_HOR >= 0){
        glUniform1fv(PIXEL_OFFSET_HOR, sizeof(pixOffset)/sizeof(float), pixOffset);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Vertical Blur Shader
    ///////////////////////////////////////////////////////////////////////////////
    if (! ( program = ProgramManagerObj->Program( ( char * )"VerticalBlur" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"VerticalBlur" );
        ProgramManagerObj->AddProgram( program );
    }
    
    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( VERTICAL_BLUR_SHADER_PRG, GL_FRAGMENT_SHADER	);
    
    /////////// Simple Vertex & vertical Blur Fragment shader //////////
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

    SCENETEX = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "RenderTex" );
    PIXEL_OFFSET_VERT = ProgramManagerObj->ProgramGetUniformLocation(program,(char*)"PixOffset[0]");
    GAUSSIAN_WEIGHT_VERT = ProgramManagerObj->ProgramGetUniformLocation(program,(char*)"Weight[0]");
    SCREEN_COORD_X_VERT = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "ScreenCoordX" );

    // dlgmlals3 요게 액티브 되어있어서 그런듯..
    //glUniform1i(SCENETEX, 1);
    glUniform1i(SCENETEX, 0);

    memset(gWeight, 0, sizeof(float)*FILTER_SIZE);
    sum         = 0;
    gWeight[0]  = Bloom::GaussianEquation(0, sigma);
    sum = gWeight[0];
    for(int i = 1; i<FILTER_SIZE; i++){
        gWeight[i] = Bloom::GaussianEquation(i, sigma);
        sum += 2*gWeight[i];
    }
    
    for(int i = 0; i<FILTER_SIZE; i++){
        gWeight[i] = gWeight[i] / sum;
    }
    
    if (GAUSSIAN_WEIGHT_VERT >= 0){
        glUniform1fv(GAUSSIAN_WEIGHT_VERT, sizeof(gWeight)/sizeof(float), gWeight);
    }
    
    for(int i = 0; i<FILTER_SIZE; i++){
        pixOffset[i] = float(i)*PIXEL_INTERVAL;
    }
    
    if (PIXEL_OFFSET_VERT >= 0){
        glUniform1fv(PIXEL_OFFSET_VERT, sizeof(pixOffset)/sizeof(float), pixOffset);
    }

    // Final Bloom Shader
    if (! ( program = ProgramManagerObj->Program( ( char * )"Bloom" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"Bloom" );
        ProgramManagerObj->AddProgram( program );
    }
    
    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( BLOOM_SHADER_PRG, GL_FRAGMENT_SHADER	);
    
    /////////// Simple Vertex & Bloom shader //////////
    m = reserveCache( VERTEX_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }
    
    m = reserveCache( BLOOM_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }
    
    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );
    glUseProgram( program->ProgramID );


    // Bloom Shader
    if (! ( program = ProgramManagerObj->Program( ( char * )"FinalBloom" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"FinalBloom" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( FINAL_BLOOM_SHADER_PRG, GL_FRAGMENT_SHADER	);

    /////////// Simple Vertex & Bloom shader //////////
    m = reserveCache( VERTEX_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }

    m = reserveCache( FINAL_BLOOM_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );
    glUseProgram( program->ProgramID );
    return;
}

void SimpleTexture::RenderSimple() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_CULL_FACE);
    glUseProgram(program->ProgramID);

    glBindVertexArray(SimpleVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SimpleTexture::Render()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_CULL_FACE);
    glUseProgram(program->ProgramID);
    
    TransformObj->TransformPushMatrix();
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, quad2);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);

    glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
    TransformObj->TransformPopMatrix();
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SimpleTexture::TouchEventDown( float x, float y )
{
    ApplyShader(HorizontalBlurShader);
    glUniform1f(SCREEN_COORD_X_HOR, x);
    
    ApplyShader(VerticalBlurShader);
    glUniform1f(SCREEN_COORD_X_VERT, x);
    lastX = x;
    lastY = y;
}

void SimpleTexture::TouchEventMove( float x, float y )
{
    ApplyShader(HorizontalBlurShader);
    glUniform1f(SCREEN_COORD_X_HOR, x);
    
    ApplyShader(VerticalBlurShader);
    glUniform1f(SCREEN_COORD_X_VERT, x);

    rotationX += (x - lastX) * PACE;
    rotationY += (y - lastY) * PACE;
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
    else if (shader == BloomShader){
        program = ProgramManagerObj->Program( ( char * )"Bloom" );
    }
    else if (shader == SimpleShader) {
        program = ProgramManagerObj->Program( ( char * )"SimpleShader" );
    }
    else if (shader == FinalShader){
        program = ProgramManagerObj->Program( ( char * )"FinalBloom" );
    }
    glUseProgram( program->ProgramID );
    return program;
}