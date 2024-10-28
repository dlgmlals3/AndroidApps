#ifndef SQUARE_H
#define SQUARE_H

#include "Model.h"
#include "glutils.h"
#include "Renderer.h"

class Square : public Model
{
private:
    void CreateVBO();
    void ClearVBO();
    GLuint bufferId;
    GLuint indexId;

    GLuint positionAttribHandle;
    GLuint colorAttribHandle;

    // 화면 range -1 ~ 1 사이
    const GLfloat vertices[12] = {
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
    };

    const GLfloat colors[12] = {
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };

    const GLuint indexes[12] = { // 오른손 법칙
            0, 2, 1,
            0, 3, 2
    };

public:
    Square( Renderer* parent = 0);
    ~Square();

    void InitModel();
    void Render();

    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
};

#endif