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
#define VERTEX_SHADER_PRG			( char * )"CubeVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"CubeFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/CubeVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/CubeFragment.glsl"
#endif

// Namespace used
using std::ifstream;
using std::ostringstream;

bool Animate            = false; // dlgmlals3
static int dimension    = 10;
static float distance   = 5.0;
static bool op          = true;
GLuint vAo            = 0;
GLuint vId              = 0;
GLuint iId              = 0;
int size                = 0;

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

GLushort cubeIndices[] = {0,3,1, 3,2,1,   // 36 of indices
                          7,4,6, 4,5,6,
                          4,0,5, 0,1,5,
                          3,7,2, 7,6,2,
                          1,2,5, 2,6,5,
                          3,0,7, 0,4,7};

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

Cube::Cube( Renderer* parent )
{
    if (!parent)
        return;

    MapRenderHandler	= parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    modelType 			= CubeType;

    // VBO 생성
    glGenBuffers(1, &vId);
    glGenBuffers(1, &iId);

    // 버텍스 VBO 설정
    size = 24 * sizeof(float);
    glBindBuffer( GL_ARRAY_BUFFER, vId );
    glBufferData( GL_ARRAY_BUFFER, size + size, 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,			size,	cubeVerts );
    glBufferSubData( GL_ARRAY_BUFFER, size,			size,	cubeColors );
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 인덱스 VBO 설정
    unsigned short indexSize = sizeof( unsigned short ) * 36;
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iId );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, indexSize,	cubeIndices );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
    LOGI("Init Model");
    if (! ( program = ProgramManagerObj->Program( (char *)"Cube" ))){
        program = ProgramManagerObj->ProgramInit( (char *)"Cube" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);

    CACHE *m = reserveCache( VERTEX_SHADER_PRG, true );

    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }

    m = reserveCache( FRAGMENT_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }

    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );

    glUseProgram( program->ProgramID );
    mvp    = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"MODELVIEWPROJECTIONMATRIX" );
    attribVertex   = ProgramManagerObj->ProgramGetVertexAttribLocation(program, (char*)"VertexPosition");
    attribColor    = ProgramManagerObj->ProgramGetVertexAttribLocation(program, (char*)"VertexColor");
    LOGI("InitModel location : %d %d\n", attribColor, attribVertex);

    // VAO 바인딩 - 여기서부터 모든 설정이 VAO에 저장됨
    glGenVertexArrays(1, &vAo);
    glBindVertexArray(vAo);

    glBindBuffer(GL_ARRAY_BUFFER, vId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iId);

    // 버텍스 버퍼 어트리뷰트 설정
    glEnableVertexAttribArray(attribColor);
    glEnableVertexAttribArray(attribVertex);
    glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)size);

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    // GL_ELEMENT_ARRAY_BUFFER는 GL_ARRAY_BUFFER와는 다르게, VAO에 영구적으로 바인딩되기 때문에 언바인딩 할필요 없음.
    // GL_ELEMENT_ARRAY_BUFFER에 바인딩된 인덱스 버퍼는 VAO에 종속됩니다. 즉, 특정 VAO가 바인딩될 때마다 해당 VAO에 바인딩된 인덱스 버퍼가 함께 활성화됩니다.
    glBindVertexArray(0); // 여기까지 설정이 다 저장됌.

    return;
}

static float k = 0;

void Cube::Render()
{
    glEnable( GL_DEPTH_TEST );
    glUseProgram( program->ProgramID );
    if(distance > 5)
        op = true;
    if(distance < 2.0)
        op = false;

    if (Animate){
        if(op)
            distance -= 0.1;
        else
            distance += 0.1;
    }

    // TransformObj->TransformTranslate(0.0f, 0.0f, 30.f);
    // 행렬
    TransformObj->TransformRotate(k++, 1, 1, 1);
    // TransformObj->PrintMatrixMode();
    RenderCubeOfCubes();
}

void Cube::RenderCube()
{
    glBindVertexArray(vAo);
    glUniformMatrix4fv( mvp, 1, GL_FALSE,(float*)TransformObj->TransformGetModelViewProjectionMatrix() );
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    glBindVertexArray(0);
}

void Cube::RenderCubeOfCubes()
{
    RenderCube(); // dlgmlals3
    /*
    TransformObj->TransformTranslate(-distance*dimension/2,  -distance*dimension/2, -distance*dimension/2);
    for (int i = 0; i < dimension; i++){
        TransformObj->TransformTranslate(distance,  0.0, 0.0);
        TransformObj->TransformPushMatrix();

        for (int j = 0; j < dimension; j++){
            TransformObj->TransformTranslate(0.0,  distance, 0.0);
            TransformObj->TransformPushMatrix();

            for (int k = 0; k < dimension; k++){
                 TransformObj->TransformTranslate(0.0,  0.0, distance);
                 RenderCube();
            }
            TransformObj->TransformPopMatrix();
        }
        TransformObj->TransformPopMatrix();
    }
    */
}

