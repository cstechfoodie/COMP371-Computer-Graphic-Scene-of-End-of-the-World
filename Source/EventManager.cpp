//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "EventManager.h"
#include "Renderer.h"
#include "World.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;
int width=1280;
int length=760;
double xposition;
double yposition;
double button_check = 1186;
double button_check2 = 1248;
int button_5=0;
int button_4=0;
int button_3=0;
int button_2=0;
int button_1=0;
// Time
double EventManager::sLastFrameTime = glfwGetTime();
float  EventManager::sFrameTime = 0.0f;

// Mouse
double EventManager::sLastMousePositionX = 0.0f;
float  EventManager::sMouseDeltaX = 0.0f;
double EventManager::sLastMousePositionY = 0.0f;
float  EventManager::sMouseDeltaY = 0.0f;




// Window
GLFWwindow* EventManager::spWindow = nullptr;


void EventManager::Initialize()
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(-1);
	}
		
	glfwWindowHint(GLFW_SAMPLES, 4);
    
#if defined(PLATFORM_OSX)
    // OS X would use legacy opengl by default, and wouldn't be able to load shaders
    // This is the proper way to setup GLFW to use modern OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
    
#else
    // Allow older laptops to run the framework, even though, our framework
    // is compatible with OpenGL 3.3 (which removed many deprecated features)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
#endif
    
    
	// Open a window and create its OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, 0);
	spWindow = glfwCreateWindow(1280, 760, "COMP371 - Assignment Framework", nullptr, nullptr);

	if (spWindow == nullptr)
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		exit(-1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(spWindow, GLFW_STICKY_KEYS, GL_TRUE);

	// Initial mouse position
	glfwPollEvents();
	double x, y;
	glfwGetCursorPos(spWindow, &x, &y);

	sLastMousePositionX = x;
	sLastMousePositionY = y;

	// Initial time
	sLastFrameTime = glfwGetTime();
    srand((unsigned int) time(nullptr));
    
}

void EventManager::Shutdown()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	spWindow = nullptr;
}

void EventManager::Update()
{
	// Update inputs / events
	glfwPollEvents();

	// Update mouse position
	double x, y;
	glfwGetCursorPos(spWindow, &x, &y);
	sMouseDeltaX = static_cast<float>( x - sLastMousePositionX );
	sMouseDeltaY = static_cast<float>( y - sLastMousePositionY );
	sLastMousePositionX = x;
	sLastMousePositionY = y;

	// Update frame time
	double currentTime = glfwGetTime();
	sFrameTime = static_cast<float>(currentTime - sLastFrameTime);
	sLastFrameTime = currentTime;
}

float EventManager::GetFrameTime()
{
	return sFrameTime;
}

bool EventManager::ExitRequested()
{
	return glfwGetKey(spWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(spWindow);
}

GLFWwindow* EventManager::GetWindow()
{
	return spWindow;
}

float EventManager::GetMouseMotionX()
{
	return sMouseDeltaX;
}

float EventManager::GetMouseMotionY()
{
	return sMouseDeltaY;
}

void EventManager::EnableMouseCursor()
{
	glfwSetInputMode(spWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    EventManager::Update();
}

void EventManager::DisableMouseCursor()
{
	glfwSetInputMode(spWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

float EventManager::GetRandomFloat(float min, float max)
{
    float value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    return min + value*(max - min);
    

}

void EventManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
    std::cout << xpos << " : " << ypos << std::endl;
    xposition = xpos;
    yposition = ypos;
}

void EventManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>22 && yposition<58)
    {
        button_1=1;
        std::cout << "Button 1" << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>135 && yposition<172)
    {
        button_2=1;
        std::cout << "Button 2" << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>247 && yposition<285)
    {
        button_3=1;
        std::cout << "Button 3" << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>362 && yposition<402)
    {
        button_4=1;
        std::cout << "Button 4" << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>475 && yposition<513)
    {
        button_5=1;
        std::cout << "Button 5:"<< button_5 << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        EventManager::EnableMouseCursor();
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xposition>button_check && xposition<button_check2 && yposition>702 && yposition<743)
    {
        EventManager::DisableMouseCursor();
    }
    
   
}

