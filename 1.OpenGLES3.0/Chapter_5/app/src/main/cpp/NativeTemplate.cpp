// OpenGL ES 2.0 Cookcook code

#include "NativeTemplate.h"
#ifdef __APPLE__
#include "renderer.h"
#else
#include "../Scene/renderer.h"
#endif
#include <time.h>
#include <string>

static clock_t clickTimeDiff;

void SetModelPath(std::string path){
    Renderer::Instance().SetModelPath(path);
}

bool GraphicsInit()
{
    Renderer::Instance().initializeRenderer();
    return true;
}

bool GraphicsResize( int width, int height )
{
    Renderer::Instance().resize(width, height);
    return true;
}

bool GraphicsRender()
{
    Renderer::Instance().setUpProjection();
    Renderer::Instance().render();
    return true;
}

void TouchEventDown( float x, float y )
{
    /*if(abs(clickTimeDiff - clock()) < 200000){ //1000 milli second
        Renderer::Instance().TouchEventDoubleClick( x, y );
    }
    else{
        Renderer::Instance().TouchEventDown( x, y );
    }*/
    Renderer::Instance().TouchEventDown( x, y );
    clickTimeDiff = clock();
}

void TouchEventMove( float x, float y )
{
    Renderer::Instance().TouchEventMove( x, y );
}

void TouchEventRelease( float x, float y )
{
    Renderer::Instance().TouchEventRelease( x, y );
}


#ifdef __ANDROID__

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_init( JNIEnv *env, jobject obj, jstring FilePath )
{
	setenv( "FILESYSTEM", env->GetStringUTFChars( FilePath, NULL ), 1 );
	GraphicsInit();
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_resize( JNIEnv *env, jobject obj, jint width, jint height)
{
	GraphicsResize( width, height );
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_step(JNIEnv * env, jobject obj)
{
	GraphicsRender();
    //renderFrame();
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventStart(JNIEnv * env, jobject obj, float x, float y )
{
	TouchEventDown(x ,y);
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventMove(JNIEnv * env, jobject obj, float x, float y )
{
	TouchEventMove(x ,y);
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventRelease(JNIEnv * env, jobject obj, float x, float y )
{
	TouchEventRelease(x ,y);
}

#endif