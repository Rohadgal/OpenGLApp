#include "Shader.h"

Shader::Shader() {
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode) {
	compileShader(vertexCode, fragmentCode);
}

void Shader::createFromFiles(const char* vertexLocation, const char* fragmentLocation) {
	std::string vertexString = readFile(vertexLocation);
	std::string fragmentString = readFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	compileShader(vertexCode, fragmentCode);
}

std::string Shader::readFile(const char* fileLocation) {
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesn't exist.\n", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::addShader(GLuint t_program, const char* t_shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = t_shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(t_shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	// For debugging purposes
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s' \n", shaderType, eLog);
		return;
	}

	glAttachShader(t_program, theShader);
}

void Shader::compileShader(const char* vertexCode, const char* fragmentCode) {
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Shader creation failed!");
		return;
	}

	addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	// For debugging purposes
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s' \n", eLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s' \n", eLog);
		return;
	}

	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformView = glGetUniformLocation(shaderID, "view");
	uniformAmbientColour = glGetUniformLocation(shaderID, "directionalLight.colour");
	uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.ambientIntensity");
	uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
	uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.diffuseIntensity");
}

GLuint Shader::GetProjectionLocation() {
	return uniformProjection;
}

GLuint Shader::GetModelLocation() {
	return uniformModel;
}

GLuint Shader::GetViewLocation() {
	return uniformView;
}

GLuint Shader::GetAmbientColourLocation() {
	return uniformAmbientColour;
}

GLuint Shader::GetDiffuseIntensityLocation() {
	return uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation() {
	return uniformDirection;
}

GLuint Shader::GetAmbientIntensityLocation() {
	return uniformAmbientIntensity;
}

void Shader::useShader() {
	if (shaderID == 0) {
		printf("Missing shader\n");
		return;
	}
	glUseProgram(shaderID);
}

void Shader::clearShader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

Shader::~Shader() {
	clearShader();
}