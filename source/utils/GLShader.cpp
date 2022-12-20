// Original file : ESIEE
// Modified by : Efflam O.

//#include "stdafx.h"
#include "GLShader.h"
//#define GLEW_STATIC
#include "GL/glew.h"

#include "../Camera.h"
#include "../LightSource.h"

#include <fstream>
#include <iostream>

unsigned int GLShader::cameraUBO = 0;
unsigned int GLShader::lightUBO = 0;


bool ValidateShader(GLuint shader)
{
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) {
        char *infoLog = new char[1 + infoLen];
        glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
        std::cout << "Shader compilation:" << infoLog;
        delete[] infoLog;
    }

    if (!compiled)
	{
		// on supprime le shader object car il est inutilisable
		glDeleteShader(shader);
		return false;
	}

	return true;
}

bool GLShader::LoadVertexShader(const char* filename)
{
	// 1. Charger le fichier en memoire
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
	if (!fin.good()) {
		std::cout << "Error! Could not open " << filename << "!\n";
		return false;
	}

	fin.seekg(0, std::ios::end);
	uint32_t length = (uint32_t)fin.tellg();
	fin.seekg(0, std::ios::beg);
	char* buffer = nullptr;
	buffer = new char[length + 1];
	buffer[length] = '\0';
	fin.read(buffer, length);

	// 2. Creer le shader object
	m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_VertexShader, 1, &buffer, nullptr);
	// 3. Le compiler
	glCompileShader(m_VertexShader);
	// 4. Nettoyer
	delete[] buffer;
	fin.close();	// non obligatoire ici
	
	// 5. 
	// verifie le status de la compilation
	return ValidateShader(m_VertexShader);
}

bool GLShader::LoadGeometryShader(const char* filename)
{
	// 1. Charger le fichier en memoire
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
    if (!fin.good()) {
        std::cout << "Error! Could not open " << filename << "!\n";
        return false;
    }
    fin.seekg(0, std::ios::end);
	uint32_t length = (uint32_t)fin.tellg();
	fin.seekg(0, std::ios::beg);
	char* buffer = nullptr;
	buffer = new char[length + 1];
	buffer[length] = '\0';
	fin.read(buffer, length);

	// 2. Creer le shader object
	m_GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(m_GeometryShader, 1, &buffer, nullptr);
	// 3. Le compiler
	glCompileShader(m_GeometryShader);
	// 4. Nettoyer
	delete[] buffer;
	fin.close();	// non obligatoire ici

					// 5. 
					// verifie le status de la compilation
	return ValidateShader(m_GeometryShader);
}

bool GLShader::LoadFragmentShader(const char* filename)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
    if (!fin.good()) {
        std::cout << "Error! Could not open " << filename << "!\n";
        return false;
    }
    fin.seekg(0, std::ios::end);
	uint32_t length = (uint32_t)fin.tellg();
	fin.seekg(0, std::ios::beg);
	char* buffer = nullptr;
	buffer = new char[length + 1];
	buffer[length] = '\0';
	fin.read(buffer, length);

	// 2. Creer le shader object
	m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_FragmentShader, 1, &buffer, nullptr);
	// 3. Le compiler
	glCompileShader(m_FragmentShader);
	// 4. Nettoyer
	delete[] buffer;
	fin.close();	// non obligatoire ici
	
	// 5. 
	// verifie le status de la compilation
	return ValidateShader(m_FragmentShader);
}

bool GLShader::Create()
{
	m_Program = glCreateProgram();
	glAttachShader(m_Program, m_VertexShader);
	if (m_GeometryShader)
		glAttachShader(m_Program, m_GeometryShader);
	glAttachShader(m_Program, m_FragmentShader);
	glLinkProgram(m_Program);

	int32_t linked = 0;
	int32_t infoLen = 0;
	// verification du statut du linkage
	glGetProgramiv(m_Program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = new char[infoLen + 1];

			glGetProgramInfoLog(m_Program, infoLen, NULL, infoLog);
			std::cout << "Erreur de lien du programme: " << infoLog << std::endl;

			delete(infoLog);
		}

		glDeleteProgram(m_Program);

		return false;
	}

	InitUBO();

	glUseProgram(m_Program);
    auto blockIndex = glGetUniformBlockIndex(m_Program, "Camera");
    glUniformBlockBinding(m_Program, blockIndex, 1);
	blockIndex = glGetUniformBlockIndex(m_Program, "Light");
	glUniformBlockBinding(m_Program, blockIndex, 2);

    return true;
}

void GLShader::Destroy()
{
	DestroyUBO();

	glDetachShader(m_Program, m_VertexShader);
	glDetachShader(m_Program, m_FragmentShader);
	glDetachShader(m_Program, m_GeometryShader);
	glDeleteShader(m_GeometryShader);
	glDeleteShader(m_VertexShader);
	glDeleteShader(m_FragmentShader);
	glDeleteProgram(m_Program);
}

void GLShader::InitUBO() {
	if(cameraUBO != 0) /* UBO already initialised */
		return;

	glGenBuffers(1, &cameraUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Camera::CameraMatrices), NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO);

	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSource::LightData), NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLShader::DestroyUBO() {
	if(cameraUBO == 0) /* UBO is not initialised */
		return;

	glDeleteBuffers(1, &cameraUBO);
	cameraUBO = 0;
}

unsigned int GLShader::GetCameraUBO() {
	return cameraUBO;
}

unsigned int GLShader::GetLightUBO() {
	return lightUBO;
}

//くコ:彡