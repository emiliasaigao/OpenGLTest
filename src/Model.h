#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"


class Model {
public:
	Model(std::string path) {
		loadModel(path);
	}
	void Draw(Shader& shader);
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;

private:
	std::string directory;

private:
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);
};