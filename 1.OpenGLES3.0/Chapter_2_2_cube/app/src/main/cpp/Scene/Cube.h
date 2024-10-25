#ifndef CUBE_H
#define CUBE_H

#include "Model.h"
#include "glutils.h"
#include "Renderer.h"

class Cube : public Model
{
private:
    char mvp;
    char attribVertex;
    char attribColor;

public:
    Cube( Renderer* parent = 0);
    ~Cube();
    void UseVBO();
    void UseVAO();

    void InitModel();

    void Render();
    void RenderCube();
    void RenderCubeOfCubes();
};

#endif // CUBE_H
