#version 300 es

// Vertex information
layout(location = 0) in vec4  VertexPosition;
layout(location = 1) in vec3  Normal;
uniform vec3    CameraPosition;
uniform float    RefractIndex;


// Model View Project matrix
uniform mat4    MODELVIEWPROJECTIONMATRIX;
uniform mat4    MODELMATRIX;
uniform mat3    NormalMatrix;

vec3     worldCoordPosition;
vec3     worldCoordNormal;
out vec3 refractedDirection;

vec3 refractC(vec3 I, vec3 N, float eta) {
    // 입사 광선과 법선 벡터를 정규화
    I = normalize(I);
    N = normalize(N);

    // 법선 벡터와 입사 벡터의 내적 계산
    float cosThetaI = dot(-I, N);

    // 전반사 여부 확인 (전반사 발생 시, 굴절되지 않음)
    float sinThetaTSquared = eta * eta * (1.0 - cosThetaI * cosThetaI);

    if (sinThetaTSquared > 1.0) {
        return reflect(I, N);
        //return vec3(0.0, 0.0, 0.0); // 전반사가 발생하면 (0, 0, 0) 반환
    }

    // 굴절 벡터 계산
    float cosThetaT = sqrt(1.0 - sinThetaTSquared);
    return eta * I + (eta * cosThetaI - cosThetaT) * N;
}

void main( void ) {
    worldCoordPosition = vec3( MODELMATRIX * VertexPosition );
    worldCoordNormal   = normalize(vec3( MODELMATRIX * vec4(Normal, 0.0)));
    if(worldCoordNormal.z < 0.0){
        worldCoordNormal.z = -worldCoordNormal.z;
    }
    vec3 incidenceRay   = normalize( CameraPosition - worldCoordPosition );
    refractedDirection  = -refractC(incidenceRay, worldCoordNormal, RefractIndex);
    gl_Position         = MODELVIEWPROJECTIONMATRIX * VertexPosition;
}
