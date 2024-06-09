#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader {
public:
	Shader();

	void createFromString(const char* vertexCode, const char* fragmentCode);
	void createFromFiles(const char* vertexLocation, const char* fragmentLocation);

	std::string readFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetAmbientColourLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetDirectionLocation();

	void useShader();
	void clearShader();

	~Shader();

private:
	GLuint shaderID, uniformProjection, uniformModel, uniformView,
		uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection;

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint t_program, const char* t_shaderCode, GLenum shaderType);
};

