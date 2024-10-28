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
//#define FRAGMENT_SHADER_PRG			( char * )"shader/VAOFragment.glsl"
#define VERTEX_SHADER_PRG			( char * )"shader/UniformBlockVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/UniformBlockFragment.glsl"
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
    {  0.0,  0.0,  0.0 }, //0
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

	RenderObj			= parent;
	MapRenderHandler	= parent;
	ProgramManagerObj	= parent->RendererProgramManager();
	TransformObj		= parent->RendererTransform();
	modelType 			= CubeType;

    degree = 0;
    // Create VBO
	size = 24*sizeof(float);
    glGenBuffers(1, &vId);
	glBindBuffer( GL_ARRAY_BUFFER, vId );
	glBufferData( GL_ARRAY_BUFFER, size + size, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0,			size,	cubeVerts );
	glBufferSubData( GL_ARRAY_BUFFER, size,			size,	cubeColors );

    // Create IBO
	unsigned short indexSize = sizeof( unsigned short )*36;
    glGenBuffers(1, &iId);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, indexSize,	cubeIndices );

    // Generate and Bind the VAO ID
    glGenVertexArrays(1, &Vertex_VAO_Id);
    glBindVertexArray(Vertex_VAO_Id);

    //Bind VBO ID and setup generic attributes
	glBindBuffer( GL_ARRAY_BUFFER, vId );
    glEnableVertexAttribArray(VERTEX_LOCATION);
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)size);

    // Bind IBO
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );

    // Use default VAO
    glBindVertexArray(0);
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

    CreateUniformBufferObject();

    return;
}

void Cube::CreateUniformBufferObject()
{
    // Get the index of the uniform block
    char blockIdx = glGetUniformBlockIndex(program->ProgramID, "Transformation");

    // Buffer space allocation
    GLint blockSize;
    glGetActiveUniformBlockiv(program->ProgramID, blockIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    //Bind the block index to BindPoint
    GLint bindingPoint = 0;
    glUniformBlockBinding(program->ProgramID, blockIdx, bindingPoint);

    // Create Uniform Buffer Object(UBO) Handle
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, 0, GL_DYNAMIC_DRAW);

    // Bind the UBO handle to BindPoint
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
}

void Cube::Render()
{
    glUseProgram( program->ProgramID );
	TransformObj->TransformTranslate(0.0,  0.0, -5.0);
    TransformObj->TransformRotate(degree++, 1, 1, 1);
    RenderCube();
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

void Cube::RenderCube()
{
    // LOGI("RenderCube Size  %d\n", size);
    // ENABLE UBO
    glBindBuffer( GL_UNIFORM_BUFFER, UBO );
    glm::mat4* matrixBuf = (glm::mat4*)glMapBufferRange( GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 3, GL_MAP_WRITE_BIT);
    matrixBuf[0] = *(TransformObj->TransformGetModelMatrix());
    matrixBuf[1] = *(TransformObj->TransformGetViewMatrix());
    matrixBuf[2] = *(TransformObj->TransformGetProjectionMatrix());
    glUnmapBuffer ( GL_UNIFORM_BUFFER );

    // UBO로 넘기는게 아니라 CPU에서 계산 다하고 Uniform 으로 넘기기
    glm::mat4 mvp = matrixBuf[2] * matrixBuf[1] * matrixBuf[0];
    // PrintMat4(mvp);
    char uniform = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"MODELVIEWPROJECTIONMATRIX" );
    glUniformMatrix4fv( uniform, 1, GL_FALSE,(float*) &mvp);

    glBindVertexArray(Vertex_VAO_Id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
}

void Cube::TouchEventDown( float x, float y )
{
	Animate = !Animate;
}

