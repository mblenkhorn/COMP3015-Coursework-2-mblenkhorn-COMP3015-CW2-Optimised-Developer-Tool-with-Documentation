#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include <glm/gtc/matrix_transform.hpp>
#include "helper/noisetex.h"

using glm::vec3;
using glm::mat3;
using glm::mat4;
using glm::vec4;

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), shadowMapWidth(512), shadowMapHeight(512), plane(13.0f, 10.0f, 200, 2), sphere(2.0f, 50, 50), cube(0.2f)
{
    mesh = ObjMesh::load("media/models/building.obj");
}

void SceneBasic_Uniform::initScene(int index)
{
      compile();
      intializeScene();
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/pcf.vert");
		prog.compileShader("shader/pcf.frag");
		prog.link();
        prog.use();

        solidProg.compileShader("shader/frustum.vert");
        solidProg.compileShader("shader/frustrum.frag");
        solidProg.link();

        waveProg.compileShader("shader/water_wave.vert");
        waveProg.compileShader("shader/water_wave.frag");
        waveProg.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    time = t;

    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;

    angle += 0.2f * deltaT;
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneBasic_Uniform::renderCubes() 
{
    prog.setUniform("Material.Ka", 0.0f, 0.0f, 1.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(1.0f, 0.0f, -0.5f));
    setShadowMatrices();
    cube.render();

    prog.setUniform("Material.Ka", 0.0f, 1.0f, 0.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(1.0f, 0.0f, -0.3f));
    setShadowMatrices();
    cube.render();

    prog.setUniform("Material.Ka", 1.0f, 1.0f, 0.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(1.0f, 0.0f, -0.1f));
    setShadowMatrices();
    cube.render();

    prog.setUniform("Material.Ka", 1.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.5f));
    setShadowMatrices();
    cube.render();

    prog.setUniform("Material.Ka", 1.0f, 0.0f, 1.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.3f));
    setShadowMatrices();
    cube.render();

    prog.setUniform("Material.Ka", 1.0f, 0.5f, 0.0f);
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);
    model = glm::translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.1f));
    setShadowMatrices();
    cube.render();
}

void SceneBasic_Uniform::GenerateShadowMap() 
{
    //Generate the Shadow Map
    view = lightFrustum.getViewMatrix();  //get the frustum view matrix
    projection = lightFrustum.getProjectionMatrix(); //get the frustum projection matrix
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO); //bind the shadow's FBO 
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passOneIndex); //get the subroutine called passOneIndex
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawBuildingScene();
    glCullFace(GL_BACK);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glFlush();
}

void SceneBasic_Uniform::render(int index)
{
    prog.use();

    GenerateShadowMap();

    //Render the Shadow Map 
    vec3 cameraPos(1.8f * cos(angle), 0.7f, 1.8f * sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, -0.175f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0));
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passTwoIndex);
    drawBuildingScene();
    glFinish();

    renderCubes();

    solidProg.use();
    solidProg.setUniform("frustumColour", vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidProg.setUniform("frustumMVP", projection * mv);
    lightFrustum.render();

    waveProg.use();
    drawWaves();
    renderWaves();


}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}


void SceneBasic_Uniform::intializeScene() 
{
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    angle = glm::two_pi<float>() * 0.85f;

    // Set up the framebuffer object
    setFBO();

    GLuint programHandle = prog.getHandle(); //get the shader program's handle

    //get the shadow shader's subroutines
    passOneIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth"); 
    passTwoIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

    //create a vec4 called shadowScale which is the shadow map
    shadowScale = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );

    lightPos = vec3(-2.5f, 2.0f, -2.5f); 

    //world coordinates
    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

    //perspective coordinates
    lightFrustum.setPerspective(40.0f, 1.0f, 0.1f, 100.0f);

    //mutliplies the shadowScale by the projection matrix multiplied by the view matrix
    lightPV = shadowScale * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    prog.setUniform("Light.Intensity", vec3(0.85f));
    prog.setUniform("ShadowMap", 0);
}


void SceneBasic_Uniform::setFBO() 
{
    GLfloat border[] = { 1.0f,0.0f,0.0f,0.0f }; //the border of the shadowMap
    
    //creating the buffer texture for the shadows
    GLuint depthTex;

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    //Assign depth buffer to texture index 0 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    //Create the FBO
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthTex, 0);

    //Draw the buffers
    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    }
    else {
        printf("Framebuffer is not complete.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::drawBuildingScene()
{
    vec3 color = vec3(1.0f, 0.85f, 0.55f); //create an arbitrary colour
    prog.setUniform("Material.Ka", 1.0f, 1.0f, 0.0);
    prog.setUniform("Material.Kd", color); //assign the arbitrary colour to Material.kd
    prog.setUniform("Material.Ks", vec3(0.0f));
    prog.setUniform("Material.Shininess", 1.0f);

    //render the mesh
    model = mat4(1.0f);
    setShadowMatrices();
    mesh->render();
}


void SceneBasic_Uniform::setShadowMatrices() 
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ShadowMatrix", lightPV * model);
}

void SceneBasic_Uniform::setWaveMatrices() 
{
    mat4 mv = view * model;
    waveProg.setUniform("waveModelViewMatrix", mv);
    waveProg.setUniform("waveNormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    waveProg.setUniform("waveMVP", projection * mv);
}

void SceneBasic_Uniform::drawWaves() 
{
    waveProg.use();
    waveProg.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
}

void SceneBasic_Uniform::renderWaves()
{
    waveProg.use();
    waveProg.setUniform("waveTime", time);

    waveProg.setUniform("Material.Kd", 0.0f, 0.0f, 1.0f);
    waveProg.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    waveProg.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    waveProg.setUniform("Material.Shininess", 100.0f);

    //render the plane that will be animated with waves
    model = mat4(1.0f);
    model = glm::translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    setWaveMatrices();
    plane.render();
}



