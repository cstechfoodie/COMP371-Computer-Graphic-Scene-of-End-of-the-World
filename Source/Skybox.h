#pragma once
#pragma once

#include "TextureLoader.h"
#include "CubeModel.h"

#include <glm/glm.hpp>
using namespace glm;

#include <vector>
using namespace std;

class Skybox : public CubeModel {

public:
	Skybox(vec3 size, string positiveXImagePath, string negativeXImagePath, string positiveYImagePath, string negativeYImagePath, string positiveZImagePath, string negativeZImagePath);
	virtual ~Skybox();

	void Update(float dt);
	void Draw();

private:
	struct SkyboxVertex
	{
		vec3 position;
		vec3 normal;
		vec4 color;
		vec2 textureCoordinate;
	};

	vector<SkyboxVertex> skyboxVertexBuffer;

	int skyboxPositiveXID;
	int skyboxNegativeXID;
	int skyboxPositiveYID;
	int skyboxNegativeYID;
	int skyboxPositiveZID;
	int skyboxNegativeZID;

	string skyboxFileNames[6];

	unsigned int skyboxVertexArrayID;
	unsigned int skyboxVertexBufferID;
};