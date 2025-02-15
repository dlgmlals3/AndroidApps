#include "SimpleTexture.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
#include "constant.h"

//Note: The Linux is very case sensitive so be aware of specifying correct folder and filename.
#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"CYMKVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"CYMKFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/CYMKVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/CYMKFragment.glsl"
#endif

#define VERTEX_POSITION 0
#define TEX_COORD 1

static float rotationX = 0.0;
static float rotationY = 0.0;
static float lastX = 0;
static float lastY = 0;
#define PACE 1.0
static float deltaX = 0;
static float deltaY = 0;
static float DecelerationFactor = 0.5;
int CYMKCaseType = 0;
float texCoords[8] = { 0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
};

float quad[12]     ={
    -1.0f, -1.0f,  0.0f,
    1.0f, -1.0f,  0.0f,
    -1.0f, 1.0f, -0.0f,
    1.0f, 1.0f, -0.0f
};

// Namespace used
using std::ifstream;
using std::ostringstream;

/*!
	Simple Default Constructor
 
	\param[in] None.
	\return None
 
 */
SimpleText::SimpleText( Renderer* parent )
{
    if (!parent)
        return;
    
    RendererHandler	= parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    modelType 			= ImageDemoType;
    char fname[MAX_PATH]= {""};
    
#ifdef __APPLE__
    GLUtils::extractPath( getenv( "FILESYSTEM" ), fname );
#else
    strcpy( fname, "/storage/emulated/0/Android/data/cookbook.opengles_9_5/Images/" );
#endif
    strcat( fname, "cartoon.png" );
    image = new PngImage();
    image->loadImage(fname);
    
}


/*!
	Simple Destructor
 
	\param[in] None.
	\return None
 
 */
SimpleText::~SimpleText()
{
    PROGRAM* program = NULL;
    if ( program = ProgramManagerObj->Program( ( char * )"square" ) )
    {
        ProgramManagerObj->RemoveProgram(program);
    }
}

/*!
	Initialize the scene, reserve shaders, compile and cache program
 
	\param[in] None.
	\return None
 
 */
void SimpleText::InitModel()
{
    
    if (! ( program = ProgramManagerObj->Program( ( char * )"square" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"square" );
        ProgramManagerObj->AddProgram( program );
    }
    
    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);
    
    /////////// Vertex shader //////////////////////////
    CACHE *m = reserveCache( VERTEX_SHADER_PRG, true );
    
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
        freeCache( m );
    }
    
    /////////// Fragment shader //////////////////////////
    m = reserveCache( FRAGMENT_SHADER_PRG, true );
    if( m ) {
        if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
        freeCache( m );
    }
    
    
    if( !ProgramManagerObj->ProgramLink( program, 1 ) ) exit( 3 );
    
    glUseProgram( program->ProgramID );
    
    MVP = ProgramManagerObj->ProgramGetUniformLocation( program, ( char* )"ModelViewProjectMatrix" );
    TEX = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "Tex1" );
    SCREEN_COORD_X = ProgramManagerObj->ProgramGetUniformLocation( program, (char*)"ScreenCoordX" );
    CYMK_CASE = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "caseCYMK" );

    glActiveTexture (GL_TEXTURE1);
    glUniform1i(TEX, 1);
    
    if (image) {
        glBindTexture(GL_TEXTURE_2D, image->getTextureID());
        
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program
 
	\param[in] None.
	\return None
 
 */
void SimpleText::Render()
{
    // Texture Rendering
    glUseProgram(program->ProgramID);
    
    // Disable culling
    glDisable(GL_CULL_FACE);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture (GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, image->getTextureID());
    
    TransformObj->TransformPushMatrix();
    TransformObj->TransformRotate(rotationX, 0.0, 1.0, 0.0);
    TransformObj->TransformRotate(rotationY, 1.0, 0.0, 0.0);
    
    
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, quad);
    glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
    TransformObj->TransformPopMatrix();
    
    // Draw triangle
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Manage rotation
    if ( deltaX != 0.0 ){
        if( deltaX > 0.0 ){
            deltaX -= DecelerationFactor;
        }
        else{
            deltaX += DecelerationFactor;
        }
        rotationX += deltaX;
    }
    
    if ( deltaY != 0.0 ){
        if( deltaY > 0.0 ){
            deltaY -= DecelerationFactor;
        }
        else{
            deltaY += DecelerationFactor;
        }
        rotationY += deltaY;
    }
}

void SimpleText::TouchEventDown( float x, float y )
{
    glUseProgram(program->ProgramID);
    if(CYMKCaseType++>4){
        CYMKCaseType = 0;
    }
    glUniform1i(CYMK_CASE, CYMKCaseType);
}

void SimpleText::TouchEventMove( float x, float y )
{
}

void SimpleText::TouchEventRelease( float x, float y )
{
}



