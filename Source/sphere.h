//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 22/7/14.
//
// Copyright (c) 2014 Concordia University. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
};

Vertex* GetRedSphereVertexBuffer(unsigned int &numVertices);
