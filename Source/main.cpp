//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Renderer.h"
#include "World.h"
#include "EventManager.h"
#include "Billboard.h"
#include "TextureLoader.h"
#include "shader.h"
#include "sphere.h"
// Include GLEW - OpenGL Extension Wrangler

// Include GLFW - OpenGL Framework (Multi Platform framework to Setup a window and OpenGL Context
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM - OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Water surface property
const int sizeX = 50;
const int sizeZ = 50;

// Light Coefficients
const vec3 lightColor(1.0f, 1.0f, 1.0f);
// light is at opposite of camera to bring out the specular
const vec4 lightPosition(-60, 40, -60, 0.0f); // If w = 0.0f, we have a directional light


void genWaterSurface(Vertex vertexArray[], int resolutionX, int resolutionZ, float t) {
    
    // generate the wave surface
    // generate all the points
    Vertex WaterSurfaceVertexBuffer[resolutionX * resolutionZ];
    for(int z=0; z<resolutionZ; z++){
        for(int x = 0; x < resolutionX; x++){
            WaterSurfaceVertexBuffer[z*resolutionX+x] = {vec3(x - (resolutionX/2),
                                                              (sin((float)z/2.0 + t) + sin((float)x/4.2 + t)) * 0.5,
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

int main(int argc, char*argv[])
{
	EventManager::Initialize();
	Renderer::Initialize();

	World world;    
    
	if (argc > 1)
	{
		world.LoadScene(argv[1]);
	}
	else
	{
		// TODO - You can alternate between different scenes for testing different things
		// Static Scene contains no animation
		// Animated Scene does
#if defined(PLATFORM_OSX)		
		world.LoadScene("Scenes/AnimatedSceneWithParticles.scene");
//		world.LoadScene("Scenes/AnimatedScene.scene");
//		world.LoadScene("Scenes/StaticScene.scene");
//		world.LoadScene("Scenes/CoordinateSystem.scene");
#else
		world.LoadScene("../Assets/Scenes/AnimatedSceneWithParticles.scene");
//		world.LoadScene("../Assets/Scenes/AnimatedScene.scene");
//		world.LoadScene("../Assets/Scenes/StaticScene.scene");
//		world.LoadScene("../Assets/Scenes/CoordinateSystem.scene");
#endif
	}

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("Shaders/Water.vertexshader", "Shaders/Water.fragmentshader");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "ViewTransform");
    glm::mat4 View = glm::lookAt(
                                       glm::vec3(60, 40, 60), // Camera is at (0,0,-5), in World Space
                                       glm::vec3( 0, 10, 0), // and looks at the origin
                                       glm::vec3( 0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                                       );
    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &View[0][0]);

    static Vertex vertexBuffer[(sizeX * 2 - 1) * (sizeZ - 1) + 1];
    float timeSinceStart = 0;
	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();
        // Use our shader
        glUseProgram(programID);
        // generate the water surface
        float dt = EventManager::GetFrameTime();
        timeSinceStart += dt;
        genWaterSurface(vertexBuffer, sizeX, sizeZ, timeSinceStart*4);
        
        // Draw the triangles !
        // Notice it is using TRIANGLE STRIPS which requires less vertices than triangle lists
        //for (int i =0; i < 24; i++)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexBuffer) / sizeof(Vertex));
		world.Update(dt);

		// Draw World
		world.Draw();        
	}
	while(EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}
