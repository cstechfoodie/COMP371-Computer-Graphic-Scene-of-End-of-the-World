#include "lampModel.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

LampModel::LampModel(vec3 size) : Model()
{
	// Create Vertex Buffer for all the verices of the Cube
	//vec3 halfSize = size * 0.5f;

	Vertex vertexBuffer[] = {  // position,                normal
		vec3(-0.5f, -0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),
		 vec3(0.5f, -0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),
		 vec3(0.5f,  0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),
		vec3(0.5f,  0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),
		vec3(-0.5f,  0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),
		vec3(-0.5f, -0.5f, -0.5f),  vec3(0.0f,  0.0f, -1.0f),

		vec3(-0.5f, -0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),
		vec3(0.5f, -0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),
		 vec3(0.5f,  0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),
		 vec3(0.5f,  0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),
		vec3(-0.5f,  0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),
		vec3(-0.5f, -0.5f,  0.5f),  vec3(0.0f,  0.0f,  1.0f),

		vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f),
		vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f),
		vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f),
		vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f),
		vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f),
		vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f),

		 vec3(0.5f,  0.5f,  0.5f),  vec3(1.0f,  0.0f,  0.0f),
		 vec3(0.5f,  0.5f, -0.5f),  vec3(1.0f,  0.0f,  0.0f),
		 vec3(0.5f, -0.5f, -0.5f),  vec3(1.0f,  0.0f,  0.0f),
		 vec3(0.5f, -0.5f, -0.5f),  vec3(1.0f,  0.0f,  0.0f),
		 vec3(0.5f, -0.5f,  0.5f),  vec3(1.0f,  0.0f,  0.0f),
		 vec3(0.5f,  0.5f,  0.5f),  vec3(1.0f,  0.0f,  0.0f),

		vec3(-0.5f, -0.5f, -0.5f),  vec3(0.0f, -1.0f,  0.0f),
		 vec3(0.5f, -0.5f, -0.5f),  vec3(0.0f, -1.0f,  0.0f),
		 vec3(0.5f, -0.5f,  0.5f),  vec3(0.0f, -1.0f,  0.0f),
		 vec3(0.5f, -0.5f,  0.5f),  vec3(0.0f, -1.0f,  0.0f),
		vec3(-0.5f, -0.5f,  0.5f),  vec3(0.0f, -1.0f,  0.0f),
		vec3(-0.5f, -0.5f, -0.5f),  vec3(0.0f, -1.0f,  0.0f),

		vec3(-0.5f,  0.5f, -0.5f),  vec3(0.0f,  1.0f,  0.0f),
		 vec3(0.5f,  0.5f, -0.5f),  vec3(0.0f,  1.0f,  0.0f),
		 vec3(0.5f,  0.5f,  0.5f),  vec3(0.0f,  1.0f,  0.0f),
		 vec3(0.5f,  0.5f,  0.5f),  vec3(0.0f,  1.0f,  0.0f),
		vec3(-0.5f,  0.5f,  0.5f),  vec3(0.0f,  1.0f,  0.0f),
		vec3(-0.5f,  0.5f, -0.5f),  vec3(0.0f,  1.0f,  0.0f)
	};

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);



	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                // size
		GL_FLOAT,        // type
		GL_FALSE,        // normalized?
		sizeof(Vertex), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);

}

LampModel::~LampModel()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}


void LampModel::Update(float dt)
{
	// If you are curious, un-comment this line to have spinning cubes!
	// That will only work if your world transform is correct...
	// mRotationAngleInDegrees += 90 * dt; // spins by 90 degrees per second

	Model::Update(dt);
}

void LampModel::Draw()
{
	// Draw the Vertex Buffer
	// Note this draws a unit Cube
	// The Model View Projection transforms are computed in the Vertex Shader
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, vec3(0.0,5.0,0.0));
	model = glm::scale(model, vec3(0.5f));
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &model[0][0]);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
}

bool LampModel::ParseLine(const std::vector<ci_string> &token)
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