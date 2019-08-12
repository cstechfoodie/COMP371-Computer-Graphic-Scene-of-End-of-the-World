#pragma once

#include "World.h"
#include "Renderer.h"
#include "ParsingHelper.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"

#include "CubeModel.h"
#include "SphereModel.h"
#include "Animation.h"
#include "Billboard.h"
#include <GLFW/glfw3.h>
#include "EventManager.h"
#include "TextureLoader.h"

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Model.h"
				  
#include <vector>
#include <list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>


class FireFX
{
public:
	FireFX(int type, Model* parent = nullptr);
	~FireFX();

	void Update(float dt);
	void Draw();

	std::vector<ParticleSystem*> pSystems;
	std::vector<BillboardList*> bList;

	static void loadDescriptors();

private:
	Model* parent;
	unsigned int numOfpSystems;

	static std::vector<ParticleDescriptor> descriptors;
	int id;
};
