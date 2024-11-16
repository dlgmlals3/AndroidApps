#include "Square.h"
#include "Cache.h"
//#include "Transform.h"
#include "constant.h"
#include "ShaderManager.h"
#define VERTEX_SHADER_PRG			( char * )"shader/TriangleVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/TriangleFragment.glsl"

Square::Square(Renderer *parent) {
    if (!parent)
        return;
    bufferId = -1;
    indexId = -1;

    MapRenderHandler	= parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    modelType 			= SquareType;
}

Square::~Square() {
    ClearVBO();
    PROGRAM* program = NULL;
    if ( program = ProgramManagerObj->Program( ( char * )"Square" ) )
    {
        ProgramManagerObj->RemoveProgram(program);
    }
}

void Square::InitModel() {
    if (!(program = ProgramManagerObj->Program( ( char * )"Triangle" ))) {
        program = ProgramManagerObj->ProgramInit( ( char * )"Triangle" );
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

    glClearColor (1,1,1,1);
    glClear (GL_COLOR_BUFFER_BIT);
    CreateVBO();
    return;
}

void Square::ClearVBO() {
    glDeleteBuffers(1, &bufferId);
    glDeleteBuffers(1, &indexId);
}

void Square::CreateVBO() {
    // VBO 생성 -> 바인딩 -> 버퍼에 데이터 복사
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    // 버텍스 배열 먼저 넣고 뒤에 컬러 배열 있음.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    // EBO 생성 -> 바인딩 -> 버퍼 복사
    glGenBuffers(1, &indexId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
}


void Square::Render() {
    glUseProgram( program->ProgramID );

    // 버퍼 정보를 셰이더에 알려줌
    char attrib = ProgramManagerObj->ProgramGetVertexAttribLocation(program, (char*)"VertexPosition");
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attrib);
    char attribColor = ProgramManagerObj->ProgramGetVertexAttribLocation(program, (char*)"VertexColor");
    // 버텍스 배열 이후에 컬러 배열 있음.
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));
    glEnableVertexAttribArray(attribColor);

    // 렌더링 삼각형 2개는 count : 6, 마지막 인자는 0이면 바인딩된 인덱스 버퍼 처음부터 사용.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Square::TouchEventDown(float x, float y) {
    LOGI("TouchEventDown %f %f", x, y);
}

void Square::TouchEventRelease(float x, float y) {
    LOGI("TouchEventRelease %f %f", x, y);
}

void Square::TouchEventMove(float x, float y) {
    LOGI("TouchEventRelease %f %f", x, y);
}

