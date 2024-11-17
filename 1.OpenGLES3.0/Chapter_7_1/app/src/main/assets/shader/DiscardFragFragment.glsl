#version 300 es
precision mediump float;

// **재질(Material) 관련 유니폼 변수**
uniform vec3 MaterialAmbient;    // 재질의 앰비언트 색상
uniform vec3 MaterialSpecular;   // 재질의 스펙큘러 색상
uniform vec3 MaterialDiffuse;    // 재질의 디퓨즈 색상

// **조명(Light) 관련 유니폼 변수**
uniform vec3 LightAmbient;       // 조명의 앰비언트 색상
uniform vec3 LightSpecular;      // 조명의 스펙큘러 색상
uniform vec3 LightDiffuse;       // 조명의 디퓨즈 색상

// **후면 조명 색상**
vec3 BackFaceLightDiffuse = vec3(1.0, 0.5, 0.3); // 후면 조명의 디퓨즈 색상 (주황색 계열)

// **조명 위치 및 조명 강도 유니폼 변수**
uniform vec3 LightPosition;      // 조명의 위치 (뷰 공간 기준)
uniform float ShininessFactor;   // 표면의 반짝임 강도 (샤이니니스)
uniform int toggleDiscardBehaviour; // 디스카드 동작을 제어하는 플래그

// **버텍스 셰이더로부터 전달된 입력 변수**
in vec3 nNormal;   // 정점의 변환된 법선 벡터
in vec3 eyeCoord;  // 뷰 공간에서의 정점 위치

// **출력 변수**
layout(location = 0) out vec4 FinalColor; // 최종 색상

// **Phong 조명 모델 계산 함수**
vec3 PhongShading(bool frontSide) {
    vec3 nLight = normalize(LightPosition - eyeCoord); // 정규화된 조명 벡터

    // **디퓨즈 조명 강도 계산**
    float cosAngle = 0.0;
    if (frontSide) {
        cosAngle = max(0.0, dot(nNormal, nLight)); // 정면인 경우: 법선 벡터와 조명 벡터의 내적
    } else {
        cosAngle = max(0.0, dot(-nNormal, nLight)); // 후면인 경우: 반전된 법선 벡터 사용
    }

    // **스펙큘러 조명 강도 계산**
    vec3 V = -normalize(eyeCoord);            // 뷰어(카메라) 벡터
    vec3 R = reflect(-nLight, nNormal);       // 반사 벡터 계산
    float sIntensity = pow(max(0.0, dot(R, V)), ShininessFactor);
    // - `ShininessFactor`는 표면의 반짝임 강도 조절

    // **조명 색상 계산 (앰비언트, 디퓨즈, 스펙큘러)**
    vec3 ambient = MaterialAmbient * LightAmbient;    // 앰비언트 조명
    vec3 specular = MaterialSpecular * LightSpecular * sIntensity; // 스펙큘러 조명
    vec3 diffuse;

    // **정면/후면 디퓨즈 색상 결정**
    if (frontSide) {
        diffuse = MaterialDiffuse * LightDiffuse * cosAngle; // 정면 조명
    } else {
        diffuse = MaterialDiffuse * BackFaceLightDiffuse * cosAngle; // 후면 조명
    }

    // **최종 조명 계산 결과 반환**
    return ambient + diffuse + specular;
}

// **객체의 로컬 좌표**
in vec3 ObjectCoord;

// **폴카 도트(Polka Dot) 패턴 관련 유니폼 변수**
uniform float Side;     // 사각형 크기
uniform float DotSize;  // 점(dot)의 크기


// **정면/후면 재질 색상**
uniform vec3 ModelColor, DotColor;                 // 정면: 모델 색상과 점 색상
uniform vec3 BackSideModelColor, BackSideDotColor; // 후면: 모델 색상과 점 색상

void main() {
    // **폴카 도트 패턴 생성**
    vec3 Square = vec3(Side, Side, Side);                // 사각형 크기
    float insideCircle, length;
    vec3 position = mod(ObjectCoord, Square) - Square / 2.0;
    // - `mod(ObjectCoord, Square)`: 객체 좌표를 사각형 크기로 나누어 반복 패턴 생성
    // - `- Square / 2.0`: 중심에서부터의 상대적인 위치로 변환

    // **점(dot) 내부 여부 계산**
    length = sqrt((position.x * position.x) + (position.y * position.y) + (position.z * position.z));
    insideCircle = step(length, DotSize);
    //
    // - `length`: 현재 위치가 점의 중심에서 얼마나 떨어져 있는지 계산
    // - `insideCircle`: 현재 위치가 점의 내부에 있는지 여부 (0 또는 1)

    // **디스카드(discard) 동작 제어**
    if (toggleDiscardBehaviour == 0) {
        if (insideCircle != 0.0)
        discard; // 점 내부인 경우 프래그먼트를 폐기
    } else {
        if (insideCircle == 0.0)
        discard; // 점 외부인 경우 프래그먼트를 폐기
    }

    // **정면/후면에 따른 색상 결정**
    // **렌더링 색상 변수**
    vec3 RenderColor;
    if (gl_FrontFacing) {
        RenderColor = vec3(mix(ModelColor, DotColor, insideCircle));
        FinalColor = vec4(RenderColor * PhongShading(true), 1.0);
    } else {
        RenderColor = vec3(mix(BackSideModelColor, BackSideDotColor, insideCircle));
        FinalColor = vec4(RenderColor * PhongShading(false), 1.0);
    }
    // - `mix()`: 점 내부는 `DotColor`, 점 외부는 `ModelColor`로 혼합
    // - `gl_FrontFacing`: 정면(true) 또는 후면(false) 여부 판별
    // - `PhongShading()`: 조명 계산 적용
}