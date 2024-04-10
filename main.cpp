//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;

int vecRand[10];
float fog;
float animatiePicuriDePloaie= 0.0f;
float lumina = 0.1f;
int numarModelDinamic = 0;

glm::vec3 lightPosition[4] ={
	glm::vec3(0.35f, 0.25f, -1.45f),
	glm::vec3(0.35f, 0.25f, -1.95f),
	glm::vec3(1.35f, 0.25f, -1.45f),
	glm::vec3(1.35f, 0.25f, -1.95f)
};
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 0.3f, 0.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.01f;
float masinaMerge = 0.0f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D masina;
gps::Model3D masina2;
gps::Model3D elice;
gps::Model3D helicopter;
gps::Model3D picur;
gps::Model3D scena2;
gps::Model3D dinamicModel[100];

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap =false;

GLuint textureID;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float pitch = 0.0f;
float yaw = -90.0f;
float anim1 = 0.0f, scalareNano = 1.0f;

static double lastX = 0;
static double lastY = 0;
float ambient = 0.015002, diffuse = 0.919001f, specular = 1.024001f, constant = 0.179000f, linear = 0.227000f, quadratic = 1.474995f;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
int prezentareScena;
void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		printf("Ambient: %f ;\nDiffuse: %f;\nSpecular: %f;\nQuadratic: %f;\nLinear: %f;\nConstant: %f.\n\n\n", ambient, diffuse, specular, quadratic, linear, constant);
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		scalareNano = scalareNano < 0.7f ? 1.0f : 0.5f;
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		numarModelDinamic += 1;
		dinamicModel[numarModelDinamic - 1].LoadModel("objects/masina/masina.obj");
	}
	if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS)
		cameraSpeed = cameraSpeed < 0.02f ? 0.03f : 0.01f;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		prezentareScena = 0;
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (lastX == 0 || lastY == 0)
	{
		lastX = xpos;
		lastY = ypos;
	}
	else {
		double xoffset = xpos - lastX;
		double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		const double sensitivity = 0.007f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		myCamera.rotate(yoffset, xoffset);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));

		lastX = xpos;
		lastY = ypos;
	}
}
void processMovement()
{

	if (pressedKeys[GLFW_KEY_Z])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_X])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_C])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_V])
	{
		glShadeModel(GL_SMOOTH);
	}
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 0.01f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 0.01f;

	}

	if (pressedKeys[GLFW_KEY_SPACE])
	{
		if (prezentareScena == 0)
		{
			myCamera.prezentare();
			myCamera.rotate(-0.45f, 0.00f);
		}
		if (prezentareScena < 350) {
			myCamera.rotate(0.00f, 0.0084f);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}
		else if(prezentareScena < 500)
		{

			myCamera.rotate(0.002f, 0.0f);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else if(prezentareScena <679)
		{

			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		else
		{
			prezentareScena = -1;
			printf("REPETARE prezentare!\n");
			myCamera.rotate(0.0f, 0.00f);
		}
			prezentareScena++;
	}
	myCustomShader.useShaderProgram();
	if (pressedKeys[GLFW_KEY_F]) {
		fog = 0.08f;
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
	}
	if (pressedKeys[GLFW_KEY_G]) {
		fog = 0.0f;
		glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
	}
	if (pressedKeys[GLFW_KEY_TAB])
	{
		lumina = 0.6f;
		lightColor = glm::vec3(lumina, lumina, lumina); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	if (pressedKeys[GLFW_KEY_1]) {
		ambient += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_2]) {
		ambient -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_3]) {
		diffuse += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_4]) {
		diffuse -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_5]) {
		specular += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_6]) {
		specular -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_7]) {
		quadratic += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_8]) {
		quadratic -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_9]) {
		linear += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_0]) {
		linear -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_MINUS]) {
		constant += 0.001f;

	}
	if (pressedKeys[GLFW_KEY_EQUAL]) {
		constant -= 0.001f;

	}
	if (pressedKeys[GLFW_KEY_BACKSPACE])
	{
		ambient = 0.0f;
		diffuse = 0.0f;
		specular = 0.0f;
		quadratic = 0.0f;
		linear = 0.0f;
		constant = 0.1f;
	}
	if (pressedKeys[GLFW_KEY_COMMA])
	{
		ambient = 0.015002, diffuse = 0.919001f, specular = 1.024001f, constant = 0.179000f, linear = 0.227000f, quadratic = 1.474995f;
	}
	//Ambient: 0.204002;
	//Diffuse: 1.019001;
	//Specular: 1.024001;
	//Quadratic: 0.774995;
	//Linear: 0.227000;
	//Constant: 0.179000.
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].position"), lightPosition[0].x, lightPosition[0].y, lightPosition[0].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].quadratic"), quadratic);


	//// Point light 2
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].position"), lightPosition[1].x, lightPosition[1].y, lightPosition[1].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].quadratic"), quadratic);


	// Point light 3
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].position"), lightPosition[2].x, lightPosition[2].y, lightPosition[2].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].quadratic"), quadratic);


	// Point light 4
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].position"), lightPosition[3].x, lightPosition[3].y, lightPosition[3].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].quadratic"), quadratic);

	if (pressedKeys[GLFW_KEY_UP]) {
		masinaMerge += 0.01;
	}
	if (pressedKeys[GLFW_KEY_B]) {
		lumina = 0.1f;
		lightColor = glm::vec3(lumina, lumina, lumina); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	if (pressedKeys[GLFW_KEY_N]) {
		lumina = 0.001;
		lightColor = glm::vec3(lumina, lumina, lumina); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	if (pressedKeys[GLFW_KEY_DOWN]) {
		masinaMerge -= 0.01;
	}
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initObjects() {
	nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
	masina.LoadModel("objects/masina/masina.obj");
	masina2.LoadModel("objects/masina/masina.obj");
	picur.LoadModel("objects/picur/picur.obj");
	helicopter.LoadModel("objects/elicopter/Heli_bell.obj");
	elice.LoadModel("objects/elicopter/elice.obj");
	lightCube.LoadModel("objects/moon/moon.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	scena2.LoadModel("objects/scena/scena2.obj");
}

void initShaders() {

	myCustomShader.loadShader("shaders/shaderNoReflections.vert", "shaders/shaderNoReflections.frag");
	myCustomShader.useShaderProgram();
	depthMapShader.loadShader("shaders/lightSpaceTrMatrix.vert", "shaders/lightSpaceTrMatrix.frag");
	depthMapShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}

void initSkyBox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
	mySkyBox.Load(faces);

}

void initUniforms() {
	myCustomShader.useShaderProgram();


	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	
	fog = 0.0f;
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fog"),fog);

	lightColor = glm::vec3(0.001f, 0.001f, 0.001f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(1.0f, 1.0f, 0.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view )) * lightDir));

	//set light color
	lightColor = glm::vec3(lumina, lumina, lumina); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	

	myCustomShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].position"), lightPosition[0].x, lightPosition[0].y, lightPosition[0].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].quadratic"), quadratic);


	//// Point light 2
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].position"), lightPosition[1].x, lightPosition[1].y, lightPosition[1].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].quadratic"), quadratic);


	// Point light 3
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].position"), lightPosition[2].x, lightPosition[2].y, lightPosition[2].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[2].quadratic"), quadratic);


	// Point light 4
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].position"), lightPosition[3].x, lightPosition[3].y, lightPosition[3].z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].ambient"), ambient, ambient, ambient);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].diffuse"), diffuse, diffuse, diffuse);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].specular"), specular, specular, specular);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].constant"), constant);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].linear"), linear);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[3].quadratic"), quadratic);



}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::vec3 directieLumina = glm::vec3(3.0f,4.0f,0.0f);
	lightDir = glm::mat3(glm::rotate( lightAngle, glm::vec3(0.0f, 1.0f, 0.0f))) * directieLumina;
	glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

std::vector<glm::mat4> matrice;
void drawObjects(gps::Shader shader, bool depthPass) {


	shader.useShaderProgram();

	model = glm::translate(glm::mat4(0.1f), glm::vec3(0.0f, (scalareNano > 0.6f ? 0.20f : 0.14f), 0.0f));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.12f * scalareNano));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	nanosuit.Draw(shader);

	for (int i = 0; i < numarModelDinamic; i++) {
		//printf("%d", numarModelDinamic);
		if (i != 2) {
			float fi = (float)i;
			model = glm::translate(glm::mat4(0.1f), glm::vec3(4.6f, 0.09f, -2.0f + fi / 10.0));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.13f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// do not send the normal matrix if we are rendering in the depth map
			if (!depthPass) {
				normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
				glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			}
			dinamicModel[i].Draw(shader);
		}
	}

	if (numarModelDinamic > 2) {
		model = glm::translate(glm::mat4(0.1f), glm::vec3(4.6f, masinaMerge + 0.09f , -2.0f + 2 / 10.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}
		dinamicModel[2].Draw(shader);
	}

	model = glm::translate(glm::mat4(0.1f), glm::vec3(1.6f, 0.19f, 0.6f));
	model = glm::rotate(model, glm::radians(anim1 * 360*3), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.08f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	elice.Draw(shader);
	

	model = glm::translate(glm::mat4(0.1f), glm::vec3(1.6f, 0.19f, 0.6f));
	model = glm::scale(model, glm::vec3(0.08f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	helicopter.Draw(shader);

	if (masinaMerge < -2.109998f)
	{
		masinaMerge = -2.1099f;
	}
	if (masinaMerge > 3.109998f)
	{
		masinaMerge = 3.1099f;
	}
	//printf("%f\n", masinaMerge);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.125f, 0.09f, masinaMerge));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.13f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	masina2.Draw(shader);

	//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.6f, 0.0f));

	anim1 += 0.002f;
	//printf("%f\n", anim1);
	if (anim1 > 1.0f)
	{
		anim1 = 0.0f;
	}
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.2f, 0.08f, 0.0f));
	if (anim1 > 0.5f) {
		if (anim1 < 0.8) {
			model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(-0.8f, 0.0f, anim1 - 0.5f));
		}
		else
		{
			model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(-0.8f, 0.0f, 0.3f));
		}
	}
	else
	{
		model = glm::rotate(model, glm::radians(anim1 * 100), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.8f, 0.0f, 0.0f));
	}
	model = glm::rotate(model, glm::radians(180.0f ), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.13f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	masina.Draw(shader);


	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.13f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	
	scena2.Draw(shader);

	glEnable(GL_BLEND);
	//printf("%f\n", animatiePicuriDePloaie);
	animatiePicuriDePloaie += 0.003f;
	if (animatiePicuriDePloaie > 0.1f)
	{
		animatiePicuriDePloaie = 0.0f;
	}
	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 15; y++) {
			for (int z = 0; z < 15; z++)
			{
				float auxiliar = animatiePicuriDePloaie;
				if (x % 3 == 2)
				{
					auxiliar *= 1.3f;
				}
				else if (x % 3 == 1)
				{
					auxiliar *= 1.5f;
				}
				else
				{
					if (z % 2 == 0)
					{
						auxiliar *= 2.0f;
					}
				}
				model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + x / 10.0f, 0.0f + y / 10.0f - auxiliar, 0.0f + z / 10.0f));
				model = glm::scale(model, glm::vec3(0.1f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


				//float offsetX = static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f;  // Random offset in x
				//float offsetY = static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f;  // Random offset in y
				//float offsetZ = static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f;  // Random offset in z

				//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + (x + offsetX) / 10.0f, 0.0f + (y + offsetY) / 10.0f - animatiePicuriDePloaie, 0.0f + (z + offsetZ) / 10.0f));
				//model = glm::scale(model, glm::vec3(0.1f));

				// do not send the normal matrix if we are rendering in the depth map
				if (!depthPass) {
					normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
					glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
				}

				picur.Draw(shader);
			}

		}
	}


	glDisable(GL_BLEND);

}
std::vector<glm::mat4> generateRandom3DMatrix(int size, float maxOffset) {
	std::vector<glm::mat4> matrixArray;

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			for (int z = 0; z < size; z++) {
				glm::vec3 randomOffset = glm::linearRand(glm::vec3(-maxOffset), glm::vec3(maxOffset));
				glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + randomOffset.x, y + randomOffset.y, z + randomOffset.z));
				matrixArray.push_back(matrix);
			}
		}
	}

	return matrixArray;
}
void renderScene() {

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);

	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view )) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = glm::translate(glm::mat4(1.0f), 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		mySkyBox.Draw(skyboxShader, view, projection);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}
int main(int argc, const char* argv[]) {
	matrice = generateRandom3DMatrix(15, 0.01f);


	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	setWindowCallbacks();
	initSkyBox();
	initFBO();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}