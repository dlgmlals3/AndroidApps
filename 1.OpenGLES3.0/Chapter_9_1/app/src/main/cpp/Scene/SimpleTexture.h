//
//  SimpleTexture.h
//  Reflection
//
//  Created by macbook on 1/16/14.
//  Copyright (c) 2014 macbook. All rights reserved.
//

#ifndef __SimpleTexture__
#define __SimpleTexture__

#include <iostream>

#include "Model.h"
#include "glutils.h"
#include "Renderer.h"

using namespace std;
enum ShaderType{
    SimpleShader = 0,
    HorizontalBlurShader,
    VerticalBlurShader
};

class SimpleTexture : public Model
{
    
public:
    // Constructor for ObjLoader
    SimpleTexture( Renderer* parent = 0);
    
    // Destructor for ObjLoader
    ~SimpleTexture();
    
    // Initialize our Model class
    void InitModel() {}
    void InitHorizontalModel();
    void InitVerticalModel();
    void SimpleInitModel();

    // Render the Model class
    void Render();
    void SimpleRender();

    // Apply Shader
    PROGRAM* ApplyShader(ShaderType shader = SimpleShader);
    
    // Touch Events
	void TouchEventDown( float x, float y );
	void TouchEventMove( float x, float y );
	void TouchEventRelease( float x, float y );
    
private:
    char MVP;

    char TEX_VERT;
    char GAUSSIAN_WEIGHT_VERT;
    char SCREEN_COORD_X_VERT;
    char PIXEL_OFFSET_VERT;

    char TEX_HOR;
    char GAUSSIAN_WEIGHT_HOR;
    char SCREEN_COORD_X_HOR;
    char PIXEL_OFFSET_HOR;
    char PIXELSIZE;

    char TEX;

};

#endif // OBJLOADER_H
