#include "Grid.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __APPLE__
#define VERTEX_SHADER_PRG			( char * )"GridVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"GridFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/GridVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/GridFragment.glsl"
#endif

#define VERTEX_LOCATION 0
#define XDimension 400
#define ZDimension 400
#define XDivision 100
#define ZDivision 100

GLuint vIdGrid;
GLuint iIdGrid;
GLuint Vertex_VAO_Id;
char MVP;

Grid::Grid( Renderer* parent )
{
	if (!parent)
		return;

	MapRenderHandler	= parent;
	ProgramManagerObj	= parent->RendererProgramManager();
	TransformObj		= parent->RendererTransform();
	modelType 			= GridType;

    CreateGrid(XDimension, ZDimension, XDivision, ZDivision);
}

void Grid::CreateGrid(GLfloat XDim, GLfloat ZDim, int XDiv, int ZDiv)
{
    // Calculate total vertices and indices.
    GLuint vertexNum        = 3 * (XDiv+1) * (ZDiv+1) * 2;
    GLuint indexNum         = ((XDiv+1) + (ZDiv+1)) * 2;
    
    // Allocate required space for vertex and indice location.
    GLfloat* gridVertex     = new GLfloat[ vertexNum ];
    GLushort* gridIndices   = new GLushort[ indexNum ];
    
    // Store unit division interval and half length of dimension.
    GLfloat xInterval       = XDim/XDiv;
    GLfloat zInterval       = ZDim/ZDiv;
    GLfloat xHalf           = XDim/2;
    GLfloat zHalf           = ZDim/2;
    int i                   = 0;

    // Assign vertices along X-axis.
    for( int j=0; j<XDiv+1; j++){
        gridVertex[i++] = j*xInterval - xHalf;
        gridVertex[i++] = 0.0f;
        gridVertex[i++] =  zHalf;
        gridVertex[i++] = j*xInterval - xHalf;
        gridVertex[i++] = 0.0f;
        gridVertex[i++] = -zHalf;
    }

    // Assign vertices along Z-axis.
    for( int j=0; j<ZDiv+1; j++){
        gridVertex[i++] = -xHalf;
        gridVertex[i++] = 0.0f;
        gridVertex[i++] = j*zInterval - zHalf;
        gridVertex[i++] = xHalf;
        gridVertex[i++] = 0.0f;
        gridVertex[i++] = j*zInterval - zHalf;
    }

    i = 0;
    
    // Assign indices along X-axis.
    for( int j=0; j<XDiv+1; j++ ){
        gridIndices[i++] = 2*j;
        gridIndices[i++] = 2*j+1;
    }

    // Assign indices along Z-axis.
    for( int j=0; j<ZDiv+1; j++ ){
        gridIndices[i++] = ((XDiv+1)*2) + (2*j);
        gridIndices[i++] = ((XDiv+1)*2) + (2*j+1);
    }
    
    // Create name buffer object ID
	GLuint size = vertexNum*sizeof(float);
    glGenBuffers(1, &vIdGrid);
    
    // Create VBO for Grid
	glBindBuffer( GL_ARRAY_BUFFER, vIdGrid);
	glBufferData( GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0,			size,	gridVertex );
    
    // Create IBO for Grid
	unsigned short indexSize = sizeof( unsigned short )*indexNum;
    glGenBuffers(1, &iIdGrid);
	glBindBuffer( GL_ARRAY_BUFFER, iIdGrid );
	glBufferData( GL_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, indexSize,	gridIndices );
    
    // Create Vertex Array Object
    glGenVertexArrays(1, &Vertex_VAO_Id);
    glBindVertexArray(Vertex_VAO_Id);
    
    // Create VBO  and set attribute parameters
    glBindBuffer( GL_ARRAY_BUFFER, vIdGrid );
    glEnableVertexAttribArray(VERTEX_LOCATION);
    glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIdGrid );
    
    // Make sure the VAO is not changed from outside code
    glBindVertexArray(0);
    
    // Unbind buffer
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    delete gridVertex;
    delete gridIndices;
}

Grid::~Grid()
{
	PROGRAM* program = NULL;
	if ( program = ProgramManagerObj->Program( ( char * )"GridPrg" ) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

void Grid::InitModel()
{
	if (! ( program = ProgramManagerObj->Program( (char *)"GridPrg" ))){
		program = ProgramManagerObj->ProgramInit( (char *)"GridPrg" );
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
	
    // Store uniform attribute location
    MVP = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"MODELVIEWPROJECTIONMATRIX" );
    return;
}

void Grid::Render()
{
    glEnable( GL_DEPTH_TEST );

    glUseProgram( program->ProgramID );
    static int k = 0;
    TransformObj->TransformPushMatrix();
    TransformObj->TransformRotate(20, 1, 0, 0);
    TransformObj->TransformRotate(k++, 0, 1, 0);
    TransformObj->TransformTranslate(0.0f, -10, -9);
    glUniformMatrix4fv( MVP, 1, GL_FALSE,(float*)TransformObj->TransformGetModelViewProjectionMatrix() );
    TransformObj->TransformPopMatrix();
    
    glBindVertexArray(Vertex_VAO_Id);
    glDrawElements(GL_LINES, ((XDivision+1) + (ZDivision+1) )*2, GL_UNSIGNED_SHORT, (void*)0);
    
}

void Grid::TouchEventDown( float x, float y )
{
}

