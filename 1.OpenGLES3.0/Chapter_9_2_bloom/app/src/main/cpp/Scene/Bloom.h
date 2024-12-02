//
//  Bloom.h
//
//
//  Created by macbook on 12/20/14.
//  Copyright (c) 2014 macbook. All rights reserved.
//

#ifndef __Bloom__
#define __Bloom__
#include "Model.h"
#include "glutils.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "SimpleTexture.h"

#include <iostream>
class Bloom : public Model
{
private:
    void InitModel();
    ObjLoader* objModel;
    SimpleTexture* textureQuad;
    GLuint SceneTexture, textureId2, BlurTexture, BloomTexture, DepthTexture;
    GLuint BlurTexture2;
    int DefaultFrameBuffer;
    
    GLuint SceneFbo, BlurFbo, BlurFbo2, BloomFbo;
    float textureAspect;
    float screenAspect;
public:
    Bloom( Renderer* parent = 0);
    ~Bloom();
    
    // Framebuffer related functions.
    unsigned int generateTexture(int width,int height,bool isDepth=false);
    void GenerateSceneFBO();
    void GenerateBloomFBO();
    void GenerateBlurFBO();
    void GenerateBlurFBO2();
    void ShowTextureForDebug(GLuint texture);
    // Render
    void Render();
    void RenderObj(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf, bool renderDirect = false);
    void RenderBloom(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderHorizontalBlur(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderVerticalBlur(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderFinal(GLint SceneTexture, GLint BloomTexture) ;

    // ProjectionSystem
    void SetUpPerspectiveProjection();
    void SetUpOrthoProjection();
    
    // TouchEvents
    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
    
    // Blur functions
    static float GaussianEquation(float val, float sigma);
};

#endif /* defined(__Bloom__) */
