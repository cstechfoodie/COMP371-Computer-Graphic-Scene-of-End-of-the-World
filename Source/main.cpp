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
#include <irrKlang/irrKlang.h>


//For soundtrack
using namespace irrklang;
ISoundEngine *SoundEngine = createIrrKlangDevice();



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


	//Play SoundTRack

	SoundEngine->play2D("../Assets/Music/15 Illidan.mp3", GL_TRUE);





	

	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();

		// Update World
		float dt = EventManager::GetFrameTime();
		world.Update(dt);

		// Draw World
		world.Draw();    
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_Z) == GLFW_PRESS)
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("../Assets/Music/DFO Underfoot Town Ds.mp3", GL_TRUE); 
		}
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_X) == GLFW_PRESS)
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("../Assets/Music/01-cha-la head-cha-la (tv size).mp3", GL_TRUE);
		}
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_C) == GLFW_PRESS)
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("../Assets/Music/01 - Super Mario Bros.mp3", GL_TRUE);
		}
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_V) == GLFW_PRESS)
		{
			SoundEngine->stopAllSounds();
			
		}
		

	}
	while(EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}
