//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Model.h"

class Water : public Model
{
public:
    // The vertex format could be different for different types of models
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };
    
    Water(int sizeX, int sizeZ);
    virtual ~Water(void);

    virtual void Update(float dt);
    virtual void Draw();
    void genWaterSurface(Water::Vertex vertexArray[], float t);

protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);

    
private:
    int sizeX;
    int sizeZ;
    int nVertex;
    
    // Material Coefficients
    const float ka = 1.0f;
    const float kd = 0.9f;
    const float ks = 0.8f;
    const float n = 128.0f;
    
    // Light Coefficients
    const glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    // light is at opposite of camera to bring out the specular
    const glm::vec4 lightPosition = glm::vec4(-60, 40, -60, 0.0f); // If w = 0.0f, we have a directional light
    
    // Get a handle for Light Attributes uniform
    uint LightPositionID;
    uint LightColorID;
    uint LightAttenuationID;
    
    // Get a handle for Material Attributes uniform
    uint MaterialID;

    float modelTime = 0;
    
    Vertex* vertexBuffer = NULL;
};


