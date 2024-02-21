#pragma once

#include<glad/glad.h>

#include<string>
#include<iostream>
#include<fstream>
#include<sstream>


class Shader {
public :
	unsigned int ID;
	Shader(const std::string& vertexPath, 
		const std::string& fragmentPath,
		const std::string& geometryPath = "");
	void use() {
		glUseProgram(ID);
	}
	void setBool(const std::string& name, bool value) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMat4f(const std::string& name, float* value) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
	}
	void setMat3f(const std::string& name, float* value) {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
	}
	void setVec3f(const std::string& name, float x, float y, float z) {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec3f(const std::string& name, float* v) {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), v[0], v[1], v[2]);
	}
};