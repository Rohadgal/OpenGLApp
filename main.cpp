#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "MyWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

//#include "stb_image.h"



// Window dimensions
//const GLint WIDTH = 800, HEIGHT = 800;
const float toRadians = 3.14159265f / 180.0f;

MyWindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

Light mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;


//
//bool direction = true;
//float triOffset = 0.0f;
//float triMaxOffset = 0.7f;
//float triIncrement = 0.01f;
//
//float curAngle = 0.0f;
//
//bool sizeDirection = true;
//float curSize = 0.4f;
//float maxSize = 0.8f;
//float minSize = 0.1f;

// Vertex Shader
static const char* vShader= "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2  //base
	};

	GLfloat vertices[] = { // last two floats are uv attributes
	//	x		y						 z									u		v
		-1.0f, -1.0f * float(sqrt(3)) / 3, 0.0f,							0.0f, 0.0f, // left
		0.0f, -1.0f * float(sqrt(3)) / 3, 1.0f * float(sqrt(3)) * 2 / 3,	0.5f, 0.0f,  // bottom
		1.0f, -1.0f * float(sqrt(3)) / 3, 0.0f,								1.0f, 0.0f, // right
		0.0f, 1.0f * float(sqrt(3)) * 2 / 3, 1.0f * float(sqrt(3))/ 3,		0.5f, 1.0f, //top
	};

	Mesh *obj1 = new Mesh();
	obj1->createMesh(vertices, indices, 20, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->createMesh(vertices, indices, 20, 12);
	meshList.push_back(obj2);
}


void createShaders() {
	Shader *shader1 = new Shader();
	shader1->createFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {

	mainWindow = MyWindow(800, 600);
	mainWindow.initialise();

	CreateObjects();
	createShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.loadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.loadTexture();

	mainLight = Light(1.0f, 1.0f, 0.0f, 0.3f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0, uniformAmbientColour = 0;

	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth()/mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose()){

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;


		// Get and Handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//// Moving triangle from side to side
		//if (direction) {

		//	triOffset += triIncrement;
		//}
		//else {
		//	triOffset -= triIncrement;
		//}

		//if (abs(triOffset) >= triMaxOffset) {
		//	direction = !direction;
		//}

		//curAngle += 0.5f;
		//if (curAngle >= 360) {
		//	curAngle -= 360;
		//}

		//if (direction) {
		//	curSize += 0.004f;
		//}
		//else {
		//	curSize -= 0.004f;
		//}

		//if (curSize >= maxSize || curSize <= minSize) {
		//	sizeDirection = !sizeDirection;
		//}

		// Clear window
		glClearColor(1.0f, 0.5f, 0.5f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();

		mainLight.useLight(uniformAmbientIntensity, uniformAmbientColour);

		glm::mat4 model(1.0f);
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0, 1, 0));
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		brickTexture.useTexture();
		meshList[0]->renderMesh();

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -3.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.useTexture();
		meshList[1]->renderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}