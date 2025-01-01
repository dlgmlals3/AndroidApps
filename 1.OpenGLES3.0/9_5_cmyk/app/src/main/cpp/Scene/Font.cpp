#include "Font.h"
#include "Cache.h"

#ifdef __APPLE__
#define VERTEX_SHADER_PRG			( char * )"vertexFont.vert"
#define FRAGMENT_SHADER_PRG			( char * )"fragFont.frag"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/vertexFont.vert"
#define FRAGMENT_SHADER_PRG			( char * )"shader/fragFont.frag"
#endif

#define VERTEX_POSITION 0
#define TEX_COORD 1

Font::Font(char* fileName, int Size, Renderer* parent, LanguageType Language)
{
    RendererHandler     = parent;
    ProgramManagerObj	= parent->RendererProgramManager();
    TransformObj		= parent->RendererTransform();
    languageType        = Language;
    switch(languageType)
    {
        case Arabic:
            modelType = FontArabic;
            break;
        case Thai:
            modelType = FontThai;
            break;
        case Punjabi:
            modelType = FontPunjabi;
            break;
        case Tamil:
            modelType = FontTamil;
            break;
        default:
            modelType = FontEnglish;
            break;
    }

    loadFont(fileName, Size);
    increament = 0;
}

Font::~Font() {
	PROGRAM* program = NULL;
	if ((program = ProgramManagerObj->Program((char *) "font"))) {
		ProgramManagerObj->RemoveProgram(program);
	}
}

void Font::Render() {
    /*
    TransformObj->TransformSetMatrixMode( PROJECTION_MATRIX );
    
    int w = RendererHandler->screenWidthPixel();
    int h = RendererHandler->screenHeightPixel();
    
    TransformObj->TransformLoadIdentity();
    // Left ,Right ,Bottom , Top
    TransformObj->TransformOrtho( 0, w, 0, h, -100, 100);
    
    TransformObj->TransformSetMatrixMode( VIEW_MATRIX );
    TransformObj->TransformLoadIdentity();
    
    TransformObj->TransformSetMatrixMode( MODEL_MATRIX );
    TransformObj->TransformLoadIdentity();
    */
}

void Font::InitModel() {
    if (! ( program = ProgramManagerObj->Program( ( char * )"font" ) ) )
    {
        program = ProgramManagerObj->ProgramInit( ( char * )"font" );
        ProgramManagerObj->AddProgram( program );
    }

    program->VertexShader	= ShaderManager::ShaderInit( VERTEX_SHADER_PRG,	GL_VERTEX_SHADER	);
    program->FragmentShader	= ShaderManager::ShaderInit( FRAGMENT_SHADER_PRG, GL_FRAGMENT_SHADER	);
    
    //////////////////////////////////////////////////////
    ///////////// Vertex shader //////////////////////////
    //////////////////////////////////////////////////////
    CACHE *m = reserveCache(VERTEX_SHADER_PRG, true);
    if (m) {
        if (!ShaderManager::ShaderCompile(program->VertexShader, (char *) m->buffer, 1))
            exit(1);
        freeCache(m);
    }
    
    //////////////////////////////////////////////////////
    ///////////// Fragment shader ////////////////////////
    //////////////////////////////////////////////////////
    m = reserveCache(FRAGMENT_SHADER_PRG, true);
    if (m) {
        if (!ShaderManager::ShaderCompile(program->FragmentShader,(char *) m->buffer, 1))
            exit(2);
        freeCache(m);
    }
    
    if (!ProgramManagerObj->ProgramLink(program, 1))
        exit(3);
    
    
    // Get the uniform location cache for optimization purpose
    MVP         = ProgramManagerObj->ProgramGetUniformLocation( program, (char*)"ModelViewProjectMatrix" );
    TEX         = ProgramManagerObj->ProgramGetUniformLocation( program, (char *) "Tex1" );
    FRAG_COLOR  = ProgramManagerObj->ProgramGetUniformLocation( program, (char*)"TextColor" );
}

void Font::drawGlyph(const Glyph& gi) {
    glUseProgram(program->ProgramID);

	float xmargin = float(gi.metric.width)/(float) (2*64);
	float ymargin = float(gi.metric.horiBearingY)/(float)(2*64);
    
    // Calculate the texture coordinates for the glyph rendering.
    float texCoords[8] = {
        gi.atlasX, gi.atlasY,
        gi.atlasX + texDimension, gi.atlasY,
        gi.atlasX, gi.atlasY + texDimension,
        gi.atlasX + texDimension, gi.atlasY + texDimension
    };

    // Calculate the dimension of the glyph Quad.
    float quad[12]	= {
        -0.5f, 0.5f,  0.0f,
         0.5f, 0.5f,  0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f };

	for (int i = 0; i<12;){
		quad[i]   *= (float)squareSize/2;
        quad[i+1] *= (float)squareSize/2;
        quad[i+2] *= (float)0;
		i		  += 3;
	}

    // Initialize the texture with texture unit 0
    glUniform1i(TEX, 0);
    TransformObj->TransformPushMatrix();
    TransformObj->TransformTranslate(-xmargin, ymargin, 0.0f );
    glUniformMatrix4fv(MVP, 1, GL_FALSE, (float*)TransformObj->TransformGetModelViewProjectionMatrix());
    TransformObj->TransformPopMatrix();

    // Send the vertex and texture coordinates to the shaders
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, quad);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Font::printText(char* str, GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Alpha) {
    // Initialize OpenGL ES States
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use font program
	glUseProgram(program->ProgramID);
    
    // Activate Texture unit 0 and assign the atlas texture
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlasTex);

	TransformObj->TransformPushMatrix();
    GLfloat color[4] = {Red, Green, Blue, Alpha};
    glUniform4fv(FRAG_COLOR, 1, color);

    for (const char* c = str; *c != '\0'; c++) {
        const Glyph& gi = glyphs[((unsigned long) *c)];
        TransformObj->TransformTranslate(gi.advanceHorizontal / 2.0, 0.0, 0.0);
        drawGlyph(gi);
    }
    TransformObj->TransformPopMatrix();
    return;
}

void Font::animateText(char* str, GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Alpha,
                       float radius, float rot)
{
    // Initialize OpenGL ES States
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use font program
    glUseProgram(program->ProgramID);
    
    // Activate Texture unit 0 and assign the atlas texture
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex);

    GLfloat color[4] = {Red, Green, Blue, Alpha};
    glUniform4fv(FRAG_COLOR, 1, color);

    /*int w = RendererHandler->screenWidthPixel();
    int h = RendererHandler->screenHeightPixel();
    TransformObj->TransformOrtho( 0, w, 0, h, -100, 100);
*/

    int num_segments = strlen(str);
    float theta = 0;
    int index = 1;
    TransformObj->TransformPushMatrix();
    TransformObj->TransformSetMatrixMode( MODEL_MATRIX );
    for (const char* c = str; *c != '\0'; c++, index++) {
        const Glyph& gi = glyphs[((unsigned long) *c)];
        float xOffset = gi.advanceHorizontal / 2.0;

        TransformObj->TransformTranslate(xOffset, 0.0, 0.0);
        //PrintMatrixDebug();
        drawGlyph(gi);

        /*
        theta = 2.0f * 3.1415926f * (increament++) / float(num_segments);
        TransformObj->TransformPushMatrix();
        //TransformObj->TransformRotate(rot++ , 0.0, 1.0, 0.0);
        float moveX = radius * cosf(theta);
        float moveZ = radius * sinf(theta);
        TransformObj->TransformTranslate(moveX, 0.0, moveZ);
        // draw!~!
        //LOGI("theta : %f %f\n", moveX, moveZ);
        //TransformObj->TransformRotate(-rot , 0.0, 1.0, 0.0);
        TransformObj->TransformPopMatrix();
        */
    }
    TransformObj->TransformPopMatrix();
    return;
}

void Font::PrintMatrixDebug(){
    glm::mat4* mvp = TransformObj->TransformGetModelMatrix();
    LOGI("dlgmlals3 1. mvp : %f %f %f %f\n", (*mvp)[0][0], (*mvp)[0][1], (*mvp)[0][2], (*mvp)[0][3]);
    LOGI("dlgmlals3 2. mvp : %f %f %f %f\n", (*mvp)[1][0], (*mvp)[1][1], (*mvp)[1][2], (*mvp)[1][3]);
    LOGI("dlgmlals3 3. mvp : %f %f %f %f\n", (*mvp)[2][0], (*mvp)[2][1], (*mvp)[2][2], (*mvp)[2][3]);
    LOGI("dlgmlals3 4. mvp : %f %f %f %f\n", (*mvp)[3][0], (*mvp)[3][1], (*mvp)[3][2], (*mvp)[3][3]);
}
