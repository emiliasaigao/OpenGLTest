#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "esgstl/vector.h"
#include "Texture.h"
#include "Shader.h"

using namespace esgstl;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

public:
	Mesh():vertices(),indices(),textures(){}
	Mesh(vector<Vertex> vs, vector<unsigned int> is, vector<Texture> ts)
		: vertices(vs), indices(is), textures(ts) {
		setUpMesh();
	}
	Mesh(Mesh&& rhs) noexcept
		: vertices(esgstl::move(rhs.vertices)),
		  indices(esgstl::move(rhs.indices)), 
		  textures(esgstl::move(rhs.textures)),
		  VAO(rhs.VAO),VBO(rhs.VBO),EBO(rhs.EBO){

	}
	Mesh(const Mesh& rhs)
		: vertices(rhs.vertices),indices(rhs.indices),textures(rhs.textures),
		VAO(rhs.VAO), VBO(rhs.VBO), EBO(rhs.EBO) {
	}

	Mesh& operator=(const Mesh& rhs) {
		if (&rhs != this) {
			this->vertices = rhs.vertices;
			this->indices = rhs.indices;
			this->textures = rhs.textures;
			VAO = rhs.VAO;
			VBO = rhs.VBO;
			EBO = rhs.EBO;
		}
		return *this;
	}
	Mesh& operator=(Mesh&& rhs) noexcept {
		Mesh tmp(esgstl::move(rhs));
		vertices.swap(tmp.vertices);
		indices.swap(tmp.indices);
		textures.swap(tmp.textures);
		VAO = tmp.VAO;
		VBO = tmp.VBO;
		EBO = tmp.EBO;
		return *this;
	}

	~Mesh() = default;

	void Draw(Shader& shader);

private:
	unsigned int VBO, EBO;
	void setUpMesh();
};

