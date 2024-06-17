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
#include "Material.h"

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

Material shinyMaterial;
Material dullMaterial;

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

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength,
	unsigned int normalOffset) {
	for (size_t i = 0; i < indiceCount; i += 3) {
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;

		// Normal average from the indices' normals
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	// Normalizing the normals
	for (size_t i = 0; i < verticeCount; i += vLength) {
		glm::vec3 normal(vertices[i + normalOffset], vertices[i + normalOffset + 1], vertices[i + normalOffset + 2]);
		normal = glm::normalize(normal);
		vertices[i + normalOffset] = normal.x; vertices[i + normalOffset + 1] = normal.y; vertices[i + normalOffset + 2] = normal.z;
	}
	/*for (size_t i = 0; i < verticeCount / vLength; i++) {
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}*/
}


void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2  //base
	};

	GLfloat vertices[] = { // last two floats are uv attributes
	//	x		y						 z									u		v		nx	  ny	nz
		-1.0f, -1.0f * float(sqrt(3)) / 3, 0.0f,							0.0f, 0.0f,		0.0f, 0.0f, 0.0f, // left
		0.0f, -1.0f * float(sqrt(3)) / 3, 1.0f * float(sqrt(3)) * 2 / 3,	0.5f, 0.0f,		0.0f, 0.0f, 0.0f, // bottom
		1.0f, -1.0f * float(sqrt(3)) / 3, 0.0f,								1.0f, 0.0f,		0.0f, 0.0f, 0.0f, // right
		0.0f, 1.0f * float(sqrt(3)) * 2 / 3, 1.0f * float(sqrt(3))/ 3,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f //top
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->createMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->createMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);
}


void createShaders() {
	Shader *shader1 = new Shader();
	shader1->createFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {

	mainWindow = MyWindow(1366, 768);
	mainWindow.initialise();

	CreateObjects();
	createShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.loadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.loadTexture();

	shinyMaterial = Material(1.0f, 32);
	dullMaterial = Material(0.3f, 4);

	mainLight = Light(0.7f, 1.0f, 0.3f, 0.2f,
					2.0f, -1.0f, -2.0f, 0.8f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformAmbientIntensity = 0, uniformAmbientColour = 0,
		uniformDirection = 0, uniformDiffuseIntensity = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

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
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
		uniformDirection = shaderList[0].GetDirectionLocation();
		uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		mainLight.useLight(uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0f);
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0, 1, 0));
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.useTexture();
		shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->renderMesh();

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, -3.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.useTexture();
		dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->renderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}