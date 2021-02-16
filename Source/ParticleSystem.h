//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Billboard.h"
#include "FireFX.h"

class ParticleDescriptor;
class ParticleEmitter;
class FireFX;

struct Particle
{
    Billboard billboard;
    float currentTime;
    float lifeTime;
    glm::vec3 velocity;
};

class ParticleSystem
{
public:
    ParticleSystem(ParticleEmitter* emitter, ParticleDescriptor* descriptor, FireFX* parent, unsigned int);
	~ParticleSystem();

    void Update(float dt);

private:
    float timeSinceLastParticleEmitted;
    ParticleDescriptor* mpDescriptor;
    ParticleEmitter* mpEmitter;
    std::list<Particle*> mParticleList;
    std::list<Particle*> mInactiveParticles;
    
	FireFX* parent;
	unsigned int id;
};
