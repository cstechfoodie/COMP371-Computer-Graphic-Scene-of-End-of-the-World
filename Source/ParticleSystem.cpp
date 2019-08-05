//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "ParticleSystem.h"
#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "EventManager.h"
#include "World.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

using namespace glm;

//self-defined value of PI
float PI = 3.14159265358979323846;

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, ParticleDescriptor* descriptor)
: mpDescriptor(descriptor), mpEmitter(emitter), timeSinceLastParticleEmitted(0.0f)
{
    assert(mpDescriptor != nullptr);
    assert(mpEmitter != nullptr);
    
    // Pre-allocate the maximum number of particles at a give time, according to
    // lifetime and emission rate
    int maxParticles = static_cast<int>(mpDescriptor->emissionRate * (mpDescriptor->totalLifetime + mpDescriptor->totalLifetimeRandomness)) + 1;
    
    mInactiveParticles.resize(maxParticles);
    for (std::list<Particle*>::iterator it = mInactiveParticles.begin(); it != mInactiveParticles.end(); ++it)
    {
        *it = new Particle();
    }
}

ParticleSystem::~ParticleSystem()
{
	for (std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
	{
        World::GetInstance()->RemoveBillboard(&(*it)->billboard);
		delete *it;
	}

    for (std::list<Particle*>::iterator it = mInactiveParticles.begin(); it != mInactiveParticles.end(); ++it)
    {
        delete *it;
    }
    
    mParticleList.resize(0);
	mInactiveParticles.resize(0);
}

void ParticleSystem::Update(float dt)
{
	// Emit particle according to the emission rate
	float averageTimeBetweenEmission = 1.0f / mpDescriptor->emissionRate;
	//    float randomValue = EventManager::GetRandomFloat(0.0f, averageTimeBetweenEmission);
	timeSinceLastParticleEmitted += dt;

	if (timeSinceLastParticleEmitted > averageTimeBetweenEmission && mInactiveParticles.size() > 0)
	{
		timeSinceLastParticleEmitted = 0.0f;


		// emit particle
		// transfer a particle from the inactive pool to the active pool
		Particle* newParticle = mInactiveParticles.back();
		mParticleList.push_back(newParticle);
		mInactiveParticles.pop_back();
		World::GetInstance()->AddBillboard(&newParticle->billboard);

		// Set particle initial parameters
		newParticle->billboard.position = mpEmitter->GetPosition();
		newParticle->billboard.size = mpDescriptor->initialSize + EventManager::GetRandomFloat(-1.0f, 1.0f) * mpDescriptor->initialSizeRandomness;
		newParticle->billboard.color = mpDescriptor->initialColor;
		newParticle->currentTime = 0.0f;
		newParticle->lifeTime = mpDescriptor->totalLifetime + mpDescriptor->totalLifetimeRandomness * EventManager::GetRandomFloat(-1.0f, 1.0f);
		newParticle->velocity = mpDescriptor->velocity;

		newParticle->billboard.angle = mpDescriptor->initialRotationAngle + EventManager::GetRandomFloat(0.0f, mpDescriptor->initialRotationAngleRandomness);

		// @TODO 7 - Initial Random Particle Velocity vector
		//
		// Adjust the random velocity according to a random vertical angle variation on a cone
		//
		// Step 1 : You can rotate the velocity vector by a random number between 0 and
		//          mpDescriptor->velocityAngleRandomness.
		// Step 2 : You can rotate the result in step 1 by an random angle from 0 to
		//          360 degrees about the original velocity vector

		//delta angle of random velocity
		float deltaAngle = EventManager::GetRandomFloat(0.0f, 1.0f) * mpDescriptor->velocityAngleRandomness;
		
		//Rotation angle of particle velocity
		float rotationAngle = EventManager::GetRandomFloat(0.0f, 360);

		//Radius of spherical coordinate
		float radius = pow(pow(newParticle->velocity.x, 2.0) + pow(newParticle->velocity.y, 2.0), 0.5);

		//for computing the rotational velocity from delta angle and the velocity after rotation by a random angle between 0 to 360
		vec4 orthogonalVector = vec4(radius * sin(deltaAngle * PI / 150), radius * cos(deltaAngle * PI / 150), newParticle->velocity.z, 0);
		mat4 orthogonalMatrix = rotate(mat4(1.0f), rotationAngle, mpDescriptor->velocity);
		orthogonalVector = orthogonalMatrix * orthogonalVector;

		//Update the value of particles' velocity
		newParticle->velocity = vec3(orthogonalVector[0], orthogonalVector[1], orthogonalVector[2]);

		World::GetInstance()->AddBillboard(&newParticle->billboard);
    }
    
    
    for (std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); it++)
    {
        Particle* p = *it;
		p->currentTime += dt;
        p->billboard.position += p->velocity * dt;
        
        // @TODO 6 - Update each particle parameters
        //
        // Update the velocity of the particle from the acceleration in the descriptor

		p->velocity += (mpDescriptor->acceleration * dt);
		//p->billboard.position += p->velocity * dt;

        // Update the size of the particle according to its growth

		p->billboard.size += dt * mpDescriptor->sizeGrowthVelocity;

        // Update The color is also updated in 3 phases
        //
        //
        // Phase 1 - Initial: from t = [0, fadeInTime] - Linearly interpolate between initial color and mid color
		if (p->currentTime < mpDescriptor->fadeInTime)
			p->billboard.color = mix(mpDescriptor->initialColor, mpDescriptor->midColor,
			(p->currentTime) / (mpDescriptor->fadeInTime));
		
		// Phase 2 - Mid:     from t = [fadeInTime, lifeTime - fadeOutTime] - color is mid color
		else if (p->currentTime < p->lifeTime - mpDescriptor->fadeOutTime)
			p->billboard.color = mpDescriptor->midColor;

		// Phase 3 - End:     from t = [lifeTime - fadeOutTime, lifeTime]
		else 
			p->billboard.color = mix(mpDescriptor->midColor, mpDescriptor->endColor,
			(p->currentTime - (p->lifeTime - mpDescriptor->fadeOutTime)) / (p->lifeTime - (p->lifeTime - mpDescriptor->fadeOutTime)));
                
        
        // ...
        //p->billboard.color = vec4(1.0f, 1.0f, 1.0f, 1.0f); // wrong... check required implementation above
        // ...
        
        // Do not touch code below...
        
        // Particles are destroyed if expired
        // Move from the particle to inactive list
        // Remove the billboard from the world
        if (p->currentTime > p->lifeTime)
        {
            mInactiveParticles.push_back(*it);
            
            World::GetInstance()->RemoveBillboard(&(p->billboard));
            mParticleList.remove(*it++);
        }
    }
}
