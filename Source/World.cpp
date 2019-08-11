//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

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

#include"lampModel.h"


using namespace std;
using namespace glm;

World* World::instance;

//const vec4 lightPosition(0.0f, 10.0f, 20.0f,1.0f);//add a point light here
//const vec3 lightColor(1.0f, 1.0f, 1.0f);

vec3 lightColors[5] = {
	vec3(1.0f,0.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,0.0f,1.0f),
	vec3(0.5f,0.5,0.5f),
	vec3(1.0f,0.0f,0.0f)
};
vec4 lightPositions[5] = {
	vec4(0.0f, 10.0f, 20.0f,1.0f),
	vec4(0.0f, 10.0f, -20.0f,1.0f),
	vec4(0.0f, 10.0f, 10.0f,1.0f),
	vec4(5.0f, 10.0f, 10.0f,1.0f),
	vec4(5.0f, 15.0f, 20.0f,1.0f)
};

World::World()
{
    instance = this;

	// Setup Camera
	mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCurrentCamera = 0;

    
#if defined(PLATFORM_OSX)
//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
#endif
    assert(billboardTextureID != 0);

    mpBillboardList = new BillboardList(2048, billboardTextureID);

    
    // TODO - You can un-comment out these 2 temporary billboards and particle system
    // That can help you debug billboards, you can set the billboard texture to billboardTest.png
    /*    Billboard *b = new Billboard();
     b->size  = glm::vec2(2.0, 2.0);
     b->position = glm::vec3(0.0, 3.0, 0.0);
     b->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
     
     Billboard *b2 = new Billboard();
     b2->size  = glm::vec2(2.0, 2.0);
     b2->position = glm::vec3(0.0, 3.0, 1.0);
     b2->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

     mpBillboardList->AddBillboard(b);
     mpBillboardList->AddBillboard(b2);
     */    // TMP
}

World::~World()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		delete *it;
	}

	mAnimation.clear();

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		delete *it;
	}

	mAnimationKey.clear();

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it < mParticleSystemList.end(); ++it)
    {
        delete *it;
    }
    mParticleSystemList.clear();
    
    for (vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        delete *it;
    }
    mParticleDescriptorList.clear();


	for (vector<Model*>::iterator it = lModel.begin(); it < lModel.end(); ++it)
	{
		delete *it;
	}
	lModel.clear();

    
	delete mpBillboardList;
}

World* World::GetInstance()
{
    return instance;
}

void World::Update(float dt)
{
	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}

	// Spacebar to change the shader
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_SOLID_COLOR);
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_BLUE);
	}

    // Update animation and keys
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        (*it)->Update(dt);
    }


	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);
	//get view matrix
	mat4 view=mCamera[mCurrentCamera]->GetViewMatrix();
	
	GLuint viewTransformID = glGetUniformLocation(Renderer::GetShaderProgramID(), "viewTransform");

	glUseProgram(Renderer::GetShaderProgramID());
	glUniformMatrix4fv(viewTransformID, 1, GL_FALSE, &view[0][0]);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}
    
    // Update billboards
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
    {
        (*it)->Update(dt);
    }

	for (vector<Model*>::iterator it = lModel.begin(); it < lModel.end(); ++it)
	{
		(*it)->Update(dt);
	}
    
    mpBillboardList->Update(dt);

}

void World::Draw()
{
	Renderer::BeginFrame();
	
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");

	// Send the view projection constants to the shader
	mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	// Draw models
	GLuint LightPosition1ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPositions[0]");
	GLuint LightPosition2ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPositions[1]");
	GLuint LightPosition3ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPositions[2]");
	GLuint LightPosition4ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPositions[3]");
	GLuint LightPosition5ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPositions[4]");
	GLuint LightColor1ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColors[0]");
	GLuint LightColor2ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColors[1]");
	GLuint LightColor3ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColors[2]");
	GLuint LightColor4ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColors[3]");
	GLuint LightColor5ID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColors[4]");
	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{	
		// Get a handle for Light Attributes uniform
		glUniform4f(LightPosition1ID, lightPositions[0].x, lightPositions[0].y, lightPositions[0].z, lightPositions[0].w);
		glUniform4f(LightPosition2ID, lightPositions[1].x, lightPositions[1].y, lightPositions[1].z, lightPositions[1].w);
		glUniform4f(LightPosition3ID, lightPositions[2].x, lightPositions[2].y, lightPositions[2].z, lightPositions[2].w);
		glUniform4f(LightPosition4ID, lightPositions[3].x, lightPositions[3].y, lightPositions[3].z, lightPositions[3].w);
		glUniform4f(LightPosition5ID, lightPositions[4].x, lightPositions[4].y, lightPositions[4].z, lightPositions[4].w);
		glUniform3f(LightColor1ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
		glUniform3f(LightColor2ID, lightColors[1].r, lightColors[1].g, lightColors[1].b);
		glUniform3f(LightColor3ID, lightColors[2].r, lightColors[2].g, lightColors[2].b);
		glUniform3f(LightColor4ID, lightColors[3].r, lightColors[3].g, lightColors[3].b);
		glUniform3f(LightColor5ID, lightColors[4].r, lightColors[4].g, lightColors[4].b);
		// Get a handle for Material Attributes uniform
		vec4 material = mModel[2]->GetMaterial();
		glUniform4f(MaterialID, material.x, material.y, material.z, material.w);
		vec3 attenuation = (*it)->GetAttenuation();
		glUniform3f(LightAttenuationID,attenuation.x, attenuation.y, attenuation.z);
		
		(*it)->Draw();
		

		
	}

	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());

	// Send the view projection constants to the shader
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

    Renderer::CheckForErrors();
    
    // Draw Billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mpBillboardList->Draw();
    glDisable(GL_BLEND);

	/*Renderer::SetShader(SHADER_LAMP);
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUseProgram(Renderer::GetShaderProgramID());
	for (vector<Model*>::iterator it = lModel.begin(); it < lModel.end(); ++it)
	{
		(*it)->Draw();
	}*/


	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();
}

void World::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{
			if( result == "cube" )
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);
			}
            else if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
			else if ( result == "animationkey" )
			{
				AnimationKey* key = new AnimationKey();
				key->Load(iss);
				mAnimationKey.push_back(key);
			}
			else if (result == "animation")
			{
				Animation* anim = new Animation();
				anim->Load(iss);
				mAnimation.push_back(anim);
			}
            else if (result == "particledescriptor")
            {
                ParticleDescriptor* psd = new ParticleDescriptor();
                psd->Load(iss);
                AddParticleDescriptor(psd);
            }
			/*if (result == "light")
			{
				LampModel* lamp = new LampModel();
				lamp->Load(iss);
				lModel.push_back(lamp);
			}*/
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();

	// Set Animation vertex buffers
	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}
}

Animation* World::FindAnimation(ci_string animName)
{
    for(std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        if((*it)->GetName() == animName)
        {
            return *it;
        }
    }
    return nullptr;
}

AnimationKey* World::FindAnimationKey(ci_string keyName)
{
    for(std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        if((*it)->GetName() == keyName)
        {
            return *it;
        }
    }
    return nullptr;
}

const Camera* World::GetCurrentCamera() const
{
     return mCamera[mCurrentCamera];
}

void World::AddBillboard(Billboard* b)
{
    mpBillboardList->AddBillboard(b);
}

void World::RemoveBillboard(Billboard* b)
{
    mpBillboardList->RemoveBillboard(b);
}

void World::AddParticleSystem(ParticleSystem* particleSystem)
{
    mParticleSystemList.push_back(particleSystem);
}

void World::RemoveParticleSystem(ParticleSystem* particleSystem)
{
    vector<ParticleSystem*>::iterator it = std::find(mParticleSystemList.begin(), mParticleSystemList.end(), particleSystem);
    mParticleSystemList.erase(it);
}

void World::AddParticleDescriptor(ParticleDescriptor* particleDescriptor)
{
    mParticleDescriptorList.push_back(particleDescriptor);
}

ParticleDescriptor* World::FindParticleDescriptor(ci_string name)
{
    for(std::vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        if((*it)->GetName() == name)
        {
            return *it;
        }
    }
    return nullptr;
}
