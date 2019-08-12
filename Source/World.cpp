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

World::Vertex vertexBuffer[4852];

World::World()
{
    instance = this;

	// Setup Camera
	mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCurrentCamera = 0;
    sizeX = 50;
    sizeZ = 50;
	fire1 = new FireFX();
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
    
	delete fire1;
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
    
    // Update Water Shader
    World::genWaterSurface(vertexBuffer, 25, 25, dt);
	
	GLuint viewTransformID = glGetUniformLocation(Renderer::GetShaderProgramID(), "viewTransform");

	glUseProgram(Renderer::GetShaderProgramID());
	glUniformMatrix4fv(viewTransformID, 1, GL_FALSE, &view[0][0]);  
    
	fire1->Update(dt);
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
	fire1->Draw();


	//for luo
	//Renderer::SetShader(SHADER_SQUARE);
	//glUseProgram(Renderer::GetShaderProgramID());
	//float tvertices[] = {
	//	0.5f,  0.5f, 0.0f,  // top right
	//	0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f,  // bottom left
	//	-0.5f,  0.5f, 0.0f   // top left 
	//};
	//unsigned int tindices[] = {  // note that we start from 0!
	//	0, 1, 3,  // first Triangle
	//	1, 2, 3   // second Triangle
	//};
	//unsigned int tVBO, tVAO, tEBO;
	//glGenVertexArrays(1, &tVAO);
	//glGenBuffers(1, &tVBO);
	//glGenBuffers(1, &tEBO);
	//// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	//glBindVertexArray(tVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(tvertices), tvertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tindices), tindices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glBindVertexArray(tVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	//						 //						 //glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexBuffer) / sizeof(Vertex));
    

	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();
}

void genWaterSurface(World::Vertex vertexArray[], int resolutionX, int resolutionZ, float t) {
    
    // generate the wave surface
    // generate all the points
    World::Vertex WaterSurfaceVertexBuffer[resolutionX * resolutionZ];
    for(int z=0; z<resolutionZ; z++){
        for(int x = 0; x < resolutionX; x++){
            WaterSurfaceVertexBuffer[z*resolutionX+x] = {vec3(x - (resolutionX/2),
                                                              (sin((float)z/2.0 + t) + sin((float)x/4.2 + t)) * 0.5+30,
                                                              z - (resolutionZ /2)),
                vec3(0,0,0),
                vec3(0,0,1)};
        }
    }
    
    // calculate the normals
    // every time we generate a new water, we need all the normals to point perpendicularly to the surface
    // so the light bounces on the surface, we can calculate reflection.
    // each points has a normal
    for(int z=1; z<resolutionZ-1; z++){
        for(int x = 1; x < resolutionX-1; x++){
            vec3 center = WaterSurfaceVertexBuffer[z*resolutionX+x].position;
            vec3 left = WaterSurfaceVertexBuffer[z*resolutionX+x-1].position;
            vec3 right = WaterSurfaceVertexBuffer[z*resolutionX+x+1].position;
            vec3 back = WaterSurfaceVertexBuffer[(z-1)*resolutionX+x].position;
            vec3 front = WaterSurfaceVertexBuffer[(z+1)*resolutionX+x].position;
            
            left = left - center;
            right = right - center;
            back = back - center;
            front = front - center;
            
            WaterSurfaceVertexBuffer[z*resolutionX+x].normal = normalize( glm::cross(back, left) +
                                                                         glm::cross(right, back) +
                                                                         glm::cross(front, right) +
                                                                         glm::cross(left, front));
            
        }
    }
    printf("%f, ", WaterSurfaceVertexBuffer[2*resolutionX+2].normal.y);
    
    int k = 1;
    for(int i=0; i<resolutionZ-1; i++){
        // go back to calculate the strip
        k--;
        for(int j=0; j<resolutionX; j++) {
            // logic to link all the points together in triangles shapes
            // to construct the triangle strip.
            if ((i % 2) == 0) {
                vertexArray[k] = WaterSurfaceVertexBuffer[(( i    * resolutionX) + j)];
                k++;
                vertexArray[k] = WaterSurfaceVertexBuffer[(((i+1) * resolutionX) + j)];
                k++;
            } else {
                vertexArray[k] = WaterSurfaceVertexBuffer[(( i    * resolutionX) + resolutionX - j - 1)];
                k++;
                vertexArray[k] = WaterSurfaceVertexBuffer[(((i+1) * resolutionX) + resolutionX - j - 1)];
                k++;
            }
        }
    }
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

