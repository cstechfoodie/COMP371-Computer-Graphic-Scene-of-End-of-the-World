//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "ParticleEmitter.h"
#include "Model.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

using namespace glm;

ParticleEmitter::ParticleEmitter(glm::vec3 position, Model* parent)
: mpParent(parent), mPosition(position)
{
}


// This is a point emitter, nothing is random
// As a small extra task, you could create derived classes
// for more interesting emitters such as line emitters or circular emitters
// In these cases, you would sample a random point on these shapes
glm::vec3 ParticleEmitter::GetPosition()
{
    // @TODO 8 - Position from Parented Emitter
    //
    // Return the position where the particle is emitted.
    // If the emitter is parented, the position is relative to its parent

	const Camera* cam = World::GetInstance()->GetCurrentCamera();
	mat4 viewMatrix = cam->GetViewMatrix();

	glm::vec3 position = mPosition;

	if (mpParent != nullptr) {
		position = vec3(mpParent->GetWorldMatrix()[3][0], mpParent->GetWorldMatrix()[3][1] + 8, mpParent->GetWorldMatrix()[3][2]);
	}
    
    return position;
}
