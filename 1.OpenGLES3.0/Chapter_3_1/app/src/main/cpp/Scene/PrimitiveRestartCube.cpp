#include "PrimitiveRestartCube.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
//#include "Transform.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __APPLE__
#define VERTEX_SHADER_PRG			( char * )"PrimitiveRestartVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"PrimitiveRestartFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/PrimitiveRestartVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/PrimitiveRestartFragment.glsl"
#endif

#define VERTEX_LOCATION 0
#define COLOR_LOCATION 1
GLuint vId;
GLuint iId, iIdNoPrmtvRstrt1, iIdNoPrmtvRstrt2;
int size;

// Namespace used
using std::ifstream;
using std::ostringstream;
bool usePrimitiveRestart = true;
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
    {  1.0,  1.0,  1.0 }, //0
    {  1.0,  1.0,  0.0 }, //1
    {  1.0,  1.0,  0.0 }, //2
    {  1.0,  1.0,  0.0 }, //3
    {  0.0,  1.0,  1.0 }, //4
    {  0.0,  1.0,  1.0 }, //5
    {  0.0,  1.0,  1.0 }, //6
    {  0.0,  1.0,  1.0 }, //7
};

// 36[ indices
GLushort cubeIndices[] = {
    0,3,1, 3,2,1,
    7,4,6, 4,5,6,
    4,0,5, 0,1,5,
    0xFFFF,
    3,7,2, 7,6,2,
    1,2,5, 2,6,5,
    3,0,7, 0,4,7
};
GLushort firstHalfIndices[] = {
    0,3,1, 3,2,1,
    7,4,6, 4,5,6,
    4,0,5, 0,1,5,
};

GLushort secondHalfIndices[] = {
    3,7,2, 7,6,2,
    1,2,5, 2,6,5,
    3,0,7, 0,4,7
};


/*!
	Simple Default Constructor

	\param[in] None.
	\return None

*/
Cube::Cube( Renderer* parent )
{
	if (!parent)
		return;

	RenderObj			= parent;
	MapRenderHandler	= parent;
	ProgramManagerObj	= parent->RendererProgramManager();
	TransformObj		= parent->RendererTransform();
	modelType 			= CubeType;

	LOGI("gl2: Cube constructor");

    // Create name buffer object ID
	size = 24*sizeof(float);
    glGenBuffers(1, &vId);

    // Create VBO
	glBindBuffer( GL_ARRAY_BUFFER, vId );
	glBufferData( GL_ARRAY_BUFFER, size + size, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0,			size,	cubeVerts );
	glBufferSubData( GL_ARRAY_BUFFER, size,			size,	cubeColors );

    // Create IBO
	unsigned short indexSize = sizeof( unsigned short )*37;
    glGenBuffers(1, &iId);
	glBindBuffer( GL_ARRAY_BUFFER, iId );
	glBufferData( GL_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, indexSize,	cubeIndices );

    
    // Create IBO for Non Primitive Restart vertex indices
	indexSize = sizeof( unsigned short )*18;
    glGenBuffers(1, &iIdNoPrmtvRstrt1);
	glBindBuffer( GL_ARRAY_BUFFER, iIdNoPrmtvRstrt1 );
	glBufferData( GL_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, indexSize,	firstHalfIndices );

    // Create IBO for Non Primitive Restart vertex indices
	indexSize = sizeof( unsigned short )*18;
    glGenBuffers(1, &iIdNoPrmtvRstrt2);
	glBindBuffer( GL_ARRAY_BUFFER, iIdNoPrmtvRstrt2 );
	glBufferData( GL_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, indexSize,	secondHalfIndices );
    
    // Enable the attribute locations
    glEnableVertexAttribArray(VERTEX_LOCATION);
    glEnableVertexAttribArray(COLOR_LOCATION);

    // Unbind buffer
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}


/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
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

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
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
    return;
}
static float k = 0;

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void Cube::Render()
{
    glEnable( GL_DEPTH_TEST );

    glUseProgram( program->ProgramID );
    TransformObj->TransformPushMatrix();
	TransformObj->TransformTranslate(0.0,  0.0, -5.0);
    static int rotation = 0;
    TransformObj->TransformRotate(rotation++, 1, 1, 1);
    RenderCube();
    TransformObj->TransformPopMatrix();
}

void Cube::RenderCube()
{
    glDisable(GL_CULL_FACE);
	char uniform = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"MODELVIEWPROJECTIONMATRIX" );
	if ( uniform >= 0 ){
		glUniformMatrix4fv( uniform, 1, GL_FALSE,(float*)TransformObj->TransformGetModelViewProjectionMatrix() );
	}

    //Enable the Primitive Restart Index

    //Bind the VBO
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)size);

    if (usePrimitiveRestart){
        LOGI("Using Primitive Restart - One Draw Call");
        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );
        glDrawElements(GL_TRIANGLES, 36+1, GL_UNSIGNED_SHORT, (void*)0); // Plus 1 because it has Primitive Restart Index.
    }
    else{
        LOGI("Using Primitive Restart - Two Draw Calls");
        glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIdNoPrmtvRstrt1 );
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (void*)0);

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIdNoPrmtvRstrt2 );
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (void*)0);
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

/*!
 This function handle Touch event down action.
 
 \param[in] x and y screen pixel position.
 
 \return None.
 */
void Cube::TouchEventDown( float x, float y )
{
	usePrimitiveRestart = !usePrimitiveRestart;
}

