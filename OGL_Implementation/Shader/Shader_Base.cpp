/*****************************************************************//**
 * \file   Shader_Base.cpp
 * \brief  Source code of Shader class
 * 
 * \author Kevin Pruvost (pruvostkevin0@gmail.com)
 * \date   March, 27 2022
 *********************************************************************/
#include "Shader_Base.hpp"

// GLM includes
#include <glm\gtc\type_ptr.hpp>

// Project includes
#include "OGL_Implementation\DebugInfo\Log.hpp"

/**
 * @brief Program memory for Use() because it is a heavy process
*/
static GLuint programUsed = -1;

Shader_Base::Shader_Base(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Shader Program
	__program = glCreateProgram();
	glAttachShader(__program, vertex);
	glAttachShader(__program, fragment);
	glLinkProgram(__program);
	// Print linking errors if any
	glGetProgramiv(__program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(__program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

Shader_Base::~Shader_Base()
{
	if (__program == programUsed) programUsed = (GLuint)(-1);
	glDeleteProgram(__program);
}

GLuint Shader_Base::Program() const
{
	return __program;
}

void Shader_Base::Use() const
{
	if (programUsed != __program)
	{
		programUsed = __program;
		glUseProgram(__program);
	}
}

void Shader_Base::AddGlobalUbo(const GLuint bindingPoint, const char * bindingPointName) const
{
	GLuint id = glGetUniformBlockIndex(__program, bindingPointName);
	glUniformBlockBinding(__program, id, bindingPoint);
}

void Shader_Base::SetUniformInt(const GLchar * uniformName, const GLint nb)
{
	glUniform1i(GetUniformId(uniformName), nb);
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const GLfloat nb)
{
	glUniform1f(GetUniformId(uniformName), nb);
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const glm::vec2 & nbs)
{
	glUniform2fv(GetUniformId(uniformName), 1, glm::value_ptr(nbs));
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const GLfloat nb1, const GLfloat nb2)
{
	glUniform2f(GetUniformId(uniformName), nb1, nb2);
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const glm::vec3 & nbs)
{
	glUniform3fv(GetUniformId(uniformName), 1, glm::value_ptr(nbs));
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const GLfloat nb1, const GLfloat nb2, const GLfloat nb3)
{
	glUniform3f(GetUniformId(uniformName), nb1, nb2, nb2);
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const glm::vec4 & nbs)
{
	glUniform4fv(GetUniformId(uniformName), 1, glm::value_ptr(nbs));
}

void Shader_Base::SetUniformFloat(const GLchar * uniformName, const GLfloat nb1, const GLfloat nb2, const GLfloat nb3, const GLfloat nb4)
{
	glUniform4f(GetUniformId(uniformName), nb1, nb2, nb2, nb3);
}

void Shader_Base::SetUniformMatrix4f(const GLchar * uniformName, const glm::mat4 & mat)
{
	glUniformMatrix4fv(GetUniformId(uniformName), 1, GL_FALSE, glm::value_ptr(mat));
}

GLuint Shader_Base::GetUniformId(const GLchar * uniformName)
{
	if (__uniformIds.contains(uniformName))
		return __uniformIds[uniformName];
	auto pair = __uniformIds.emplace(uniformName, glGetUniformLocation(__program, uniformName));
	return pair.second;
}
