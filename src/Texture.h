#pragma once
#include<glad/glad.h>

#include<string>
#include<iostream>
#include"stb_image.h"

class Texture {
public :
	unsigned int ID;
	std::string type;
	std::string path;
	Texture():ID(0),type(),path(){}
	Texture(const std::string& texPath):ID(0),type(),path(texPath) {
		ID = loadTexture(path.c_str());
	}

	static Texture CreateTexture(unsigned int width, unsigned int height,
		GLint internalformat = GL_RGB, GLenum formal = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
	static unsigned int loadTexture(const char* path);
};