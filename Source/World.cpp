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

#include "BSpline.hpp"
#include "BSplineCamera.h"
#include "Water.h"

using namespace std;
using namespace glm;

World* World::instance;
extern int button_5;
extern int button_4;
extern int button_2;
extern int button_1;
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

	particles = vector<FireFX*>();
    FireFX::loadDescriptors();
    water = new Water(40,40);
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
    


	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);
	//get view matrix
	mat4 view=mCamera[mCurrentCamera]->GetViewMatrix();
	
	GLuint viewTransformID = glGetUniformLocation(Renderer::GetShaderProgramID(), "viewTransform");

	glUseProgram(Renderer::GetShaderProgramID());
	glUniformMatrix4fv(viewTransformID, 1, GL_FALSE, &view[0][0]);  
    
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}

	for(auto fire : particles)
	{
		fire->Update(dt);
	}
    
    water->Update(dt);
    
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
	//GLuint LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightPosition");
	//GLuint LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColor");
	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
    
    
    if(button_5==1){
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{	
		// Get a handle for Light Attributes uniform
		//glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
		//glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
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
		 vec4 material = mModel[1]->GetMaterial();

		glUniform4f(MaterialID, material.x, material.y, material.z, material.w);
		vec3 attenuation = (*it)->GetAttenuation();
		glUniform3f(LightAttenuationID,attenuation.x, attenuation.y, attenuation.z);
		
		(*it)->Draw();
	}
    }
    
    if(button_4==1){
        for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
        {
            // Get a handle for Light Attributes uniform
            //glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
            //glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
            glUniform4f(LightPosition1ID, lightPositions[0].x, lightPositions[0].y, lightPositions[0].z, lightPositions[0].w);
            glUniform4f(LightPosition2ID, lightPositions[1].x, lightPositions[1].y, lightPositions[1].z, lightPositions[1].w);
            glUniform4f(LightPosition3ID, lightPositions[2].x, lightPositions[2].y, lightPositions[2].z, lightPositions[2].w);
            glUniform4f(LightPosition4ID, lightPositions[3].x, lightPositions[3].y, lightPositions[3].z, lightPositions[3].w);
            glUniform4f(LightPosition5ID, lightPositions[4].x, lightPositions[4].y, lightPositions[4].z, lightPositions[4].w);
            glUniform3f(LightColor1ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor2ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor3ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor4ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor5ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            // Get a handle for Material Attributes uniform
            vec4 material = mModel[1]->GetMaterial();
            
            glUniform4f(MaterialID, material.x, material.y, material.z, material.w);
            vec3 attenuation = (*it)->GetAttenuation();
            glUniform3f(LightAttenuationID,attenuation.x, attenuation.y, attenuation.z);
            
            (*it)->Draw();
        }
    }

    if(button_1==1){
        for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
        {
            // Get a handle for Light Attributes uniform
            //glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
            //glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
            glUniform4f(LightPosition1ID, lightPositions[1].x, lightPositions[1].y, lightPositions[1].z, lightPositions[0].w);
            glUniform4f(LightPosition2ID, lightPositions[0].x, lightPositions[0].y, lightPositions[0].z, lightPositions[1].w);
            glUniform4f(LightPosition3ID, lightPositions[4].x, lightPositions[4].y, lightPositions[4].z, lightPositions[2].w);
            glUniform4f(LightPosition4ID, lightPositions[3].x, lightPositions[3].y, lightPositions[3].z, lightPositions[3].w);
            glUniform4f(LightPosition5ID, lightPositions[2].x, lightPositions[2].y, lightPositions[2].z, lightPositions[4].w);
            glUniform3f(LightColor1ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor2ID, lightColors[1].r, lightColors[1].g, lightColors[1].b);
            glUniform3f(LightColor3ID, lightColors[2].r, lightColors[2].g, lightColors[2].b);
            glUniform3f(LightColor4ID, lightColors[3].r, lightColors[3].g, lightColors[3].b);
            glUniform3f(LightColor5ID, lightColors[4].r, lightColors[4].g, lightColors[4].b);
            // Get a handle for Material Attributes uniform
            vec4 material = mModel[1]->GetMaterial();
            
            glUniform4f(MaterialID, material.x, material.y, material.z, material.w);
            vec3 attenuation = (*it)->GetAttenuation();
            glUniform3f(LightAttenuationID,attenuation.x, attenuation.y, attenuation.z);
            
            (*it)->Draw();
        }
    }
    
    if(button_2==1){
        for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
        {
            // Get a handle for Light Attributes uniform
            //glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
            //glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
            glUniform4f(LightPosition1ID, lightPositions[0].x, lightPositions[0].y, lightPositions[0].z, lightPositions[0].w);
            glUniform4f(LightPosition2ID, lightPositions[1].x, lightPositions[1].y, lightPositions[1].z, lightPositions[1].w);
            glUniform4f(LightPosition3ID, lightPositions[2].x, lightPositions[2].y, lightPositions[2].z, lightPositions[2].w);
            glUniform4f(LightPosition4ID, lightPositions[3].x, lightPositions[3].y, lightPositions[3].z, lightPositions[3].w);
            glUniform4f(LightPosition5ID, lightPositions[4].x, lightPositions[4].y, lightPositions[4].z, lightPositions[4].w);
            glUniform3f(LightColor1ID, lightColors[0].r, lightColors[0].g, lightColors[0].b);
            glUniform3f(LightColor2ID, lightColors[0].r, lightColors[1].g, lightColors[0].b);
            glUniform3f(LightColor3ID, lightColors[0].r, lightColors[2].g, lightColors[0].b);
            glUniform3f(LightColor4ID, lightColors[0].r, lightColors[3].g, lightColors[0].b);
            glUniform3f(LightColor5ID, lightColors[0].r, lightColors[4].g, lightColors[0].b);
            // Get a handle for Material Attributes uniform
            vec4 material = mModel[1]->GetMaterial();
            
            glUniform4f(MaterialID, material.x, material.y, material.z, material.w);
            vec3 attenuation = (*it)->GetAttenuation();
            glUniform3f(LightAttenuationID,attenuation.x, attenuation.y, attenuation.z);
            
            (*it)->Draw();
        }
    }
    
	//water
   // Renderer::SetShader(SHADER_WATER);
    // glUseProgram(Renderer::GetShaderProgramID());
    water->Draw();
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
	for (auto fire : particles)
	{
		fire->Draw();
	}

    
	//for luo
	Renderer::SetShader(SHADER_SQUARE1);
	glUseProgram(Renderer::GetShaderProgramID());
	float tvertices[] = {
		1.0f,  1.0f, 0.0f,  // top right
		1.0f, -1.0f, 0.0f,  // bottom right
        0.8f, -1.0f, 0.0f,  // bottom left
		0.8f,  1.0f, 0.0f   // top left
	};
	unsigned int tindices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	unsigned int tVBO, tVAO, tEBO;
	glGenVertexArrays(1, &tVAO);
	glGenBuffers(1, &tVBO);
	glGenBuffers(1, &tEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(tVAO);

	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tvertices), tvertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tindices), tindices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(tVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
						 //						 //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    
    Renderer::SetShader(SHADER_SQUARE2);
    glUseProgram(Renderer::GetShaderProgramID());
    
    mat4 model = glm::mat4(1.0f);
    glm::vec3 po(0.9f,0.9f,-1.0f);
    model= glm::translate(model, po);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    float tbvertices[] = {
        1.0f,  1.0f, 0.0f,  // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left
    };
    unsigned int tbindices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int tbVBO, tbVAO, tbEBO;
    glGenVertexArrays(1, &tbVAO);
    glGenBuffers(1, &tbVBO);
    glGenBuffers(1, &tbEBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(tbVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, tbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tbvertices), tbvertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tbindices), tbindices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    

    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    //button2
    model = glm::mat4(1.0f);
    glm::vec3 poi(0.9f,0.6f,-1.0f);
    model= glm::translate(model, poi);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    
    
    model = glm::mat4(1.0f);
    glm::vec3 pois(0.9f,0.3f,-1.0f);
    model= glm::translate(model, pois);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    
    model = glm::mat4(1.0f);
    glm::vec3 poisi(0.9f,0.0f,-1.0f);
    model= glm::translate(model, poisi);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    
    
    model = glm::mat4(1.0f);
    glm::vec3 poisit(0.9f,0.0f,-1.0f);
    model= glm::translate(model, poisit);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    
    
    model = glm::mat4(1.0f);
    glm::vec3 poisiti(0.9f,-0.3f,-1.0f);
    model= glm::translate(model, poisiti);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    model = glm::mat4(1.0f);
    glm::vec3 poisitio(0.9f,-0.6f,-1.0f);
    model= glm::translate(model, poisitio);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    
    model = glm::mat4(1.0f);
    glm::vec3 poisition(0.9f,-0.9f,-1.0f);
    model= glm::translate(model, poisition);
    model= glm::scale(model, glm::vec3(0.05f));
    model= glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0,0.0,1.0));
    glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(),"WorldTransform"),1,false,&model[0][0]);
    
    glBindVertexArray(tbVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //                         //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
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

				if (cube->GetName().length() == 8) {
					particles.push_back(new FireFX(0, cube));
				}
				else {
					particles.push_back(new FireFX(1, cube));
				}
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
            else if (result == "spline")
            {
                BSpline* spline = new BSpline();
                spline->Load(iss);
                spline->CreateVertexBuffer();
                
                // FIXME: This is hardcoded: replace last camera with spline camera
                mSpline.push_back(spline);
                mCamera.pop_back();
                mCamera.push_back(new BSplineCamera(spline, 10.0f));
            }
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

