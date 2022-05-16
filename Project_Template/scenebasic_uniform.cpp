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

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), sphere(1.0f, 50, 50), shadowMapWidth(512), shadowMapHeight(512), plane(40.0f, 40.0f, 2,2){}

void SceneBasic_Uniform::initScene()
{
    compile();

	//glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	//glEnable(GL_DEPTH_TEST);

	//angle = glm::quarter_pi<float>();

	// Set up the framebuffer object
	//setupFBO();

	//GLuint programHandle = prog.getHandle();
	//passOneIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	//passTwoIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	//shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
	//	vec4(0.0f, 0.5f, 0.0f, 0.0f),
		//vec4(0.0f, 0.0f, 0.5f, 0.0f),
		//vec4(0.5f, 0.5f, 0.5f, 1.0f)
	//);

	//float c = 1.65f;
	//vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f);  // World coords
	//lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	//lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
	//lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

	//prog.setUniform("Light.Intensity", vec3(0.85f));
	//prog.setUniform("ShadowMap", 0);
}

void SceneBasic_Uniform::drawShadows() 
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	angle = glm::quarter_pi<float>();

	// Set up the framebuffer object
	setupFBO();

	GLuint programHandle = prog.getHandle();
	passOneIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	passTwoIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	float c = 1.65f;
	vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f);  // World coords
	lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
	lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

	prog.setUniform("Light.Intensity", vec3(0.85f));
	prog.setUniform("ShadowMap", 0);
}

void SceneBasic_Uniform::spitOutDepthBuffer() 
{
	int size = shadowMapWidth * shadowMapHeight;
	float* buffer = new float[size];
	unsigned char* imgBuffer = new unsigned char[size * 4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, buffer);

	for (int i = 0; i < shadowMapHeight; i++)
		for (int j = 0; j < shadowMapWidth; j++)
		{
			int imgIdx = 4 * ((i * shadowMapWidth) + j);
			int bufIdx = ((shadowMapHeight - i - 1) * shadowMapWidth) + j;

			// This is just to make a more visible image.  Scale so that
			// the range (minVal, 1.0) maps to (0.0, 1.0).  This probably should
			// be tweaked for different light configurations.
			float minVal = 0.88f;
			float scale = (buffer[bufIdx] - minVal) / (1.0f - minVal);
			unsigned char val = (unsigned char)(scale * 255);
			imgBuffer[imgIdx] = val;
			imgBuffer[imgIdx + 1] = val;
			imgBuffer[imgIdx + 2] = val;
			imgBuffer[imgIdx + 3] = 0xff;
		}

	delete[] buffer;
	delete[] imgBuffer;
	exit(1);
}

void SceneBasic_Uniform::setupFBO() 
{
	GLfloat border[] = { 1.0f, 0.0f,0.0f,0.0f };
	// The depth buffer texture
	GLuint depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the depth buffer texture to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	// Create and set up the FBO
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);

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

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/Shadow_Map.vert");
		prog.compileShader("shader/Shadow_Map.frag");
		prog.link();
		prog.use();

		solidProg.compileShader("shader/LightFrustum.vert", GLSLShader::VERTEX);
		solidProg.compileShader("shader/LightFrustum.frag", GLSLShader::FRAGMENT);
		solidProg.link();

		nightVisionProg.compileShader("shader/NightVision.vert");
		nightVisionProg.compileShader("shader/NightVision.frag");
		nightVisionProg.link();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;

	angle += 0.2f * deltaT;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneBasic_Uniform::render(int index)
{
	if (index == 0) 
	{
		prog.use();
		drawShadows();
		// Pass 1 (shadow map generation)
		view = lightFrustum.getViewMatrix();
		projection = lightFrustum.getProjectionMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passOneIndex);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.5f, 10.0f);
		drawScene();
		glCullFace(GL_BACK);
		glFlush();
		//spitOutDepthBuffer(); // This is just used to get an image of the depth buffer

		// Pass 2 (render)
		float c = 2.0f;
		vec3 cameraPos(c * 11.5f * cos(angle), c * 7.0f, c * 11.5f * sin(angle));
		view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f));
		projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passTwoIndex);
		drawScene();

		// Draw the light's frustum
		solidProg.use();
		solidProg.setUniform("Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));
		mat4 mv = view * lightFrustum.getInverseViewMatrix();
		solidProg.setUniform("MVP", projection * mv);
		lightFrustum.render();
	}
	else if(index == 1)
	{
		nightVisionProg.use();
		drawNightVision();
		passOne();
		glFlush();
		passTwo();

	}
	
}


void SceneBasic_Uniform::drawScene() 
{
	vec3 color = vec3(0.7f, 0.5f, 0.3f);

	prog.setUniform("Material.Ka", color);
	prog.setUniform("Material.Kd", 1.0f, 0.0f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Shininess", 100.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	sphere.render();

	prog.setUniform("Material.Ka", color);
	prog.setUniform("Material.Kd", 0.0f, 1.0f, 0.0f);
	prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Shininess", 100.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(2.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	sphere.render();

	prog.setUniform("Material.Kd", 0.25f, 0.25f, 0.25f);
	prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
	prog.setUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
	prog.setUniform("Material.Shininess", 1.0f);
	model = mat4(1.0f);
	setMatrices();
	plane.render();
	model = mat4(1.0f);
	model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
	setMatrices();
	plane.render();
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	plane.render();
	model = mat4(1.0f);


}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}

void SceneBasic_Uniform::setMatrices() 
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",
		glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
	prog.setUniform("ShadowMatrix", lightPV * model);
}


void SceneBasic_Uniform::drawNightVision() 
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	projection = mat4(1.0f);

	angle = glm::pi<float>() / 4.0f;

	setupNightVisionFBO();

	// Array for full-screen quad
	GLfloat verts[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	// Set up the buffers

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

	// Set up the vertex array object

	glGenVertexArrays(1, &fsQuad);
	glBindVertexArray(fsQuad);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
	glEnableVertexAttribArray(2);  // Texture coordinates

	glBindVertexArray(0);

	// Set up the subroutine indexes
	GLuint programHandle = prog.getHandle();
	passOneIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "passOne");
	passTwoIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "passTwo");

	nightVisionProg.setUniform("Width", width);
	nightVisionProg.setUniform("Height", height);
	nightVisionProg.setUniform("Radius", width / 3.5f);
	nightVisionProg.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

	noiseTex = NoiseTex::generatePeriodic2DTex(200.0f, 0.5f, 512, 512);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTex);

	nightVisionProg.setUniform("RenderTex", 0);
	nightVisionProg.setUniform("NoiseTex", 1);
}

void SceneBasic_Uniform::setupNightVisionFBO() 
{
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &renderFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

	// Create the texture object
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuf);

	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::passOne() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passOneIndex);

	view = glm::lookAt(vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

	nightVisionProg.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	nightVisionProg.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
	nightVisionProg.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
	nightVisionProg.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
	nightVisionProg.setUniform("Material.Shininess", 100.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	sphere.render();
}

void SceneBasic_Uniform::passTwo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passTwoIndex);
	model = mat4(1.0f);
	view = mat4(1.0f);
	projection = mat4(1.0f);
	setMatrices();

	// Render the full-screen quad
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::setNightVisionMatrices()
{
	mat4 mv = view * model;
	nightVisionProg.setUniform("nightVisionModelViewMatrix", mv);
	nightVisionProg.setUniform("nightVisionNormalMatrix",
		glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	nightVisionProg.setUniform("nightVisionMVP", projection * mv);
}

