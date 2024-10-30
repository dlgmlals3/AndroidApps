#include "Cube.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
//#include "Transform.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __APPLE__
#define VERTEX_SHADER_PRG			( char * )"VAOVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"VAOFragment.glsl"
#else
//#define VERTEX_SHADER_PRG			( char * )"shader/VAOVertex.glsl"
//#define FRAGMENT_SHADER_PRG		( char * )"shader/VAOFragment.glsl"
//#define VERTEX_SHADER_PRG			( char * )"shader/UniformBlockVertex.glsl"
//#define FRAGMENT_SHADER_PRG			( char * )"shader/UniformBlockFragment.glsl"
#define VERTEX_SHADER_PRG			( char * )"shader/BufferMappingVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/BufferMappingFragment.glsl"
#endif

#define VERTEX_LOCATION 0
#define COLOR_LOCATION 1
// Namespace used
bool Animate = true;
// Global Object Declaration
GLfloat  cubeVerts[][3] = {
	-1, -1, 1 , // V0
	-1, 1,  1 , // V1
	1,  1, 1 ,  // V2
	1,  -1,  1 ,// V3
	-1, -1, -1 ,// V4
	-1, 1,  -1 ,// V5
	1,  1, -1 , // V6
	1,  -1,  -1 // V7
};

GLfloat  cubeColors[][3] = {
    {  0.5,  0.5,  0.5 }, //0
    {  0.0,  0.0,  1.0 }, //1
    {  0.0,  1.0,  0.0 }, //2
    {  0.0,  1.0,  1.0 }, //3
    {  1.0,  0.0,  0.0 }, //4
    {  1.0,  0.0,  1.0 }, //5
    {  1.0,  1.0,  0.0 }, //6
    {  1.0,  1.0,  1.0 }, //7
};

GLfloat  cubeColors2[][3] = {
        {  0.5,  0.5,  0.5 }, //0
        {  0.0,  0.0,  1.0 }, //1
        {  0.0,  1.0,  0.0 }, //2
        {  0.0,  1.0,  1.0 }, //3
        {  1.0,  0.0,  0.0 }, //4
        {  1.0,  0.0,  1.0 }, //5
        {  1.0,  1.0,  0.0 }, //6
        {  1.0,  1.0,  1.0 }, //7
};

// 36[ indices
GLushort cubeIndices[] = {
    0,3,1, 3,2,1,
    7,4,6, 4,5,6,
    4,0,5, 0,1,5,
    3,7,2, 7,6,2,
    1,2,5, 2,6,5,
    3,0,7, 0,4,7
};

Cube::Cube( Renderer* parent )
{
    if (!parent)
        return;
    
    MapRenderHandler	= parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    modelType 			= CubeType;
    last                = clock();
}

Cube::~Cube()
{
	PROGRAM* program = NULL;
	if ( program = ProgramManagerObj->Program( ( char * )"Cube" ) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
	glDeleteBuffers(1, &vId);
	glDeleteBuffers(1, &iId);
}

void Cube::InitModel()
{
	if (! ( program = ProgramManagerObj->Program( (char *)"Cube" ))){
		program = ProgramManagerObj->ProgramInit( (char *)"Cube" );
		ProgramManagerObj->AddProgram( program );
	}

    //Initialize Vertex and Fragment shader
	program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
	program->FragmentShader	= ShaderManager::ShaderInit( FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);

    // Compile Vertex shader
	CACHE *m = reserveCache( VERTEX_SHADER_PRG, true );
	if( m ) {
		if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
	}

    // Compile Fragment shader
	m = reserveCache( FRAGMENT_SHADER_PRG, true );
	if( m ) {
		if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
	}

    // Link program
    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );

    glUseProgram( program->ProgramID );
    
    // Create VBO
    size = sizeof(cubeVerts);
    glGenBuffers(1, &vId);
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    glBufferData( GL_ARRAY_BUFFER, size * 2, 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,			size,	cubeVerts );
    glBufferSubData( GL_ARRAY_BUFFER, size,			size,	cubeColors );

    // Create IBO
    unsigned short indexSize = sizeof( unsigned short )*36;
    glGenBuffers(1, &iId);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, indexSize,	cubeIndices );
    
    glGenVertexArrays(1, &Vertex_VAO_Id);
    glBindVertexArray(Vertex_VAO_Id);
    
    // Create VBO  and set attribute parameters
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    glEnableVertexAttribArray(VERTEX_LOCATION);
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)size);
    
    // Bind IBO
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );
    
    // Make sure the VAO is not changed from outside code
    glBindVertexArray(0);
    
    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program
 
	\param[in] None.
	\return None
 
 */
void Cube::Render()
{
    glEnable( GL_DEPTH_TEST );
    
    glUseProgram( program->ProgramID );
    RenderCube();
    RenderCubeFixedColor();
}

void Cube::PrintMat4(const glm::mat4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            LOGI("%f ", mat[i][j]);
        }
        LOGI("\n");
    }
    LOGI("\n");
}

void Cube::RenderCubeFixedColor() {
/*    // Perform Transformation.
    TransformObj->TransformPushMatrix();
    TransformObj->TransformRotate(l++, 0.50, 0.60, 1.0);
    char uniform = ProgramManagerObj->ProgramGetUniformLocation( program, (char*)"MODELVIEWPROJECTIONMATRIX");
    if ( uniform >= 0 ){
        glUniformMatrix4fv( uniform, 1, GL_FALSE,(float*)TransformObj->TransformGetModelViewProjectionMatrix() );
    }
    TransformObj->TransformPopMatrix();*/


}

void Cube::RenderCube()
{
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    float* colorBuf = (float*)glMapBufferRange(GL_ARRAY_BUFFER, size, size, GL_MAP_WRITE_BIT);
    for (int i=0; i < sizeof(cubeColors) / sizeof(float); i++) {
        colorBuf[i] = float(rand()%255)/255;
    }
    glUnmapBuffer ( GL_ARRAY_BUFFER );

    // Perform Transformation.
    TransformObj->TransformPushMatrix();
    TransformObj->TransformRotate(l++, 0.50, 0.60, 1.0);
    char uniform = ProgramManagerObj->ProgramGetUniformLocation( program, (char*)"MODELVIEWPROJECTIONMATRIX");
    if ( uniform >= 0 ){
        glUniformMatrix4fv( uniform, 1, GL_FALSE,(float*)TransformObj->TransformGetModelViewProjectionMatrix() );
    }
    TransformObj->TransformPopMatrix();

    glBindVertexArray(Vertex_VAO_Id);
    glDrawElements(GL_POINTS, 36, GL_UNSIGNED_SHORT, (void*)0);

    /*
    두번째 렌더링
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    colorBuf = (float*)glMapBufferRange(GL_ARRAY_BUFFER, size, size, GL_MAP_WRITE_BIT);
    for (int i=0; i < sizeof(cubeColors) / sizeof(float); i++) {
        colorBuf[i] = 1.0;
    }
    glUnmapBuffer ( GL_ARRAY_BUFFER );
    glBindVertexArray(Vertex_VAO_Id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    */
}

void Cube::TouchEventDown( float x, float y )
{
	Animate = !Animate;
}

