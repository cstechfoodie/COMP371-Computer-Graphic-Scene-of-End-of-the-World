//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Water.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

using namespace glm;

Water::Water(int sizeX, int sizeZ) : Model()
{
    this->sizeX = sizeX;
    this->sizeZ = sizeZ;
    nVertex = (sizeX * 2 - 1) * (sizeZ - 1) + 1;
    vertexBuffer = new Vertex[nVertex];
}

Water::~Water()
{
    //glDeleteBuffrs(1, &mVBO);
    //glDeleteVertexArrays(1, &mVAO);
}


void Water::Update(float dt)
{
    modelTime += dt;
    this->genWaterSurface(vertexBuffer, modelTime);
}

void Water::Draw()
{
    
//    /* Draw the water surface */
//    // Get a handle for our Transformation Matrices uniform
   GLuint WorldMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
   GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
  GLuint ProjMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectonTransform");
//
   glm::mat4 World(1.0f);
  glm::mat4 View  = glm::lookAt(
                                      glm::vec3(  60, 40, 60), // Camera is at (0,0,-5), in World Space
                                      glm::vec3( 0, 10, 0), // and looks at the origin
                                      glm::vec3( 0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
                                       );
//    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldLightPosition");
    LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColor");
    LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
    
    // Get a handle for Material Attributes uniform
    MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
    glUniformMatrix4fv(WorldMatrixID, 1, GL_FALSE, &World[0][0]);
    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE, &View[0][0]);
    glUniformMatrix4fv(ProjMatrixID,  1, GL_FALSE, &Projection[0][0]);
    // Set shader constants
    glUniform4f(MaterialID, ka, kd, ks, n);
    glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
    glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
    
    // Upload Vertex Buffer to the GPU
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, nVertex*sizeof(Vertex), vertexBuffer, GL_STATIC_DRAW);
    
    // 1st attribute buffer : vertex Positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(Vertex), // stride
                          (void*)0        // array buffer offset
                          );
    
    // 2nd attribute buffer : vertex normal
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex2)
                          );
    
    
    // 3rd attribute buffer : vertex color
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, sizeof(vertexBufferID));
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(2 * sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex2)
                          );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nVertex);
    
    // delete the buffer
    glDeleteBuffers(1, &vertexBufferID);
    
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);


}


void Water::genWaterSurface(Vertex vertexArray[], float t) {
    
    
    // generate the wave surface
    // generate all the points
    Vertex WaterSurfaceVertexBuffer[sizeX * sizeZ];
    for(int z=0; z<sizeZ; z++){
        for(int x = 0; x < sizeX; x++){ {
            WaterSurfaceVertexBuffer[z*sizeX+x] = {vec3((x - (sizeX/2)),
                                                        (sin((float)z/2.0 + t) + sin((float)x/4.2 + t)) * 0.5 - 10,
                                                        (z - (sizeZ /2))),
                                                    vec3(0,0,0),
                                                    vec3(0,0,1)};
        }
            
        }
    }
    
    // calculate the normals
    // every time we generate a new water, we need all the normals to point perpendicularly to the surface
    // so the light bounces on the surface, we can calculate reflection.
    // each points has a normal
    for(int z=1; z<sizeZ-1; z++){
        for(int x = 1; x < sizeX-1; x++){
            vec3 center = WaterSurfaceVertexBuffer[z*sizeX+x].position;
            vec3 left = WaterSurfaceVertexBuffer[z*sizeX+x-1].position;
            vec3 right = WaterSurfaceVertexBuffer[z*sizeX+x+1].position;
            vec3 back = WaterSurfaceVertexBuffer[(z-1)*sizeX+x].position;
            vec3 front = WaterSurfaceVertexBuffer[(z+1)*sizeX+x].position;
            
            left = left - center;
            right = right - center;
            back = back - center;
            front = front - center;
            
            WaterSurfaceVertexBuffer[z*sizeX+x].normal = normalize( glm::cross(back, left) +
                                                                         glm::cross(right, back) +
                                                                         glm::cross(front, right) +
                                                                         glm::cross(left, front));
            
        }
    }
    //printf("%f, ", WaterSurfaceVertexBuffer[2*resolutionX+2].normal.y);
    
    int k = 1;
    for(int i=0; i<sizeZ-1; i++){
        // go back to calculate the strip
        k--;
        for(int j=0; j<sizeX; j++) {
            // logic to link all the points together in triangles shapes
            // to construct the triangle strip.
            if ((i % 2) == 0) {
                vertexArray[k] = WaterSurfaceVertexBuffer[(( i    * sizeX) + j)];
                k++;
                vertexArray[k] = WaterSurfaceVertexBuffer[(((i+1) * sizeX) + j)];
                k++;
            } else {
                vertexArray[k] = WaterSurfaceVertexBuffer[(( i    * sizeX) + sizeX - j - 1)];
                k++;
                vertexArray[k] = WaterSurfaceVertexBuffer[(((i+1) * sizeX) + sizeX - j - 1)];
                k++;
            }
        }
    }
}

bool Water::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
    else
    {
        return Model::ParseLine(token);
    }
}
