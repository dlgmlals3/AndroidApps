#ifndef CUBE_H
#define CUBE_H

#include "Model.h"
#include "glutils.h"
#include "Renderer.h"

class Cube : public Model
{
private:
    int width, height;
    GLuint UBO;
	Renderer* RenderObj;

    GLuint vId;
    GLuint iId;
    GLuint Vertex_VAO_Id;
    int size;
    float degree;

    GLuint matrixId;
    float l;
    clock_t last;
    
public:
    Cube( Renderer* parent = 0);
    ~Cube();

    void InitModel();
    
    void Render();
    void PrintMat4(const glm::mat4& mat);
    void RenderCube();
    void RenderCubeFixedColor();
    void CreateUniformBufferObject();
    void TouchEventDown( float x, float y );
};

#endif // CUBE_H
