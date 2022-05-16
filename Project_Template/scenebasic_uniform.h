#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/sphere.h"
#include "helper/plane.h";
#include "helper/frustum.h";

#include <glad/glad.h>
#include "helper/glslprogram.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    Sphere sphere;
    Plane plane;

    Frustum lightFrustum;

    float tPrev, angle;

    int shadowMapWidth, shadowMapHeight;

    glm::mat4 lightPV, shadowBias;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render(int);
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
