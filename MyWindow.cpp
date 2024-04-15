#include "MyWindow.h"

MyWindow::MyWindow() {
	width = 800;
	height = 600;
	bufferHeight = 0;
	bufferWidth = 0;
	mainWindow = nullptr;
	mouseFirstMoved = true;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

MyWindow::MyWindow(GLint windowWidth, GLint windowHeight) {
	width = windowWidth;
	height = windowHeight;
	bufferHeight = 0;
	bufferWidth = 0;
	mainWindow = nullptr;
	mouseFirstMoved = true;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

int MyWindow::initialise() {
	if (!glfwInit()) {
		printf("GLFW init failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow) {
		printf("Window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Handle Key + Mouse Input
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW init failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void MyWindow::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat MyWindow::getXChange() {
	GLfloat t_change = xChange;
	xChange = 0.0f;
	return t_change;
}

GLfloat MyWindow::getYChange() {
	GLfloat t_change = yChange;
	yChange = 0.0f;
	return t_change;
}

void MyWindow::handleKeys(GLFWwindow *window, int key, int code, int action, int mode){
	MyWindow* t_window = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			t_window->keys[key] = true;
			printf("Pressed: %d\n", key);
		}
		else if (action == GLFW_RELEASE) {
			t_window->keys[key] = false;
			printf("Released: %d\n", key);
		}
	}
}

void MyWindow::handleMouse(GLFWwindow* window, double xPos, double yPos) {
	MyWindow* t_window = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));

	if (t_window->mouseFirstMoved) {
		t_window->lastX = xPos;
		t_window->lastY = yPos;
		t_window->mouseFirstMoved = false;
	}

	t_window->xChange = xPos - t_window->lastX;
	t_window->yChange = t_window->lastY - yPos; // these can be inverted for camera 

	t_window->lastX = xPos;
	t_window->lastY = yPos;

	//printf("x:%.6f, y:%.6f\n", t_window->xChange, t_window->yChange);
}

MyWindow::~MyWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
