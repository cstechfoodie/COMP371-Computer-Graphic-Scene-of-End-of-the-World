#include "Skybox.h"
#include "Renderer.h"
#include "World.h"
#include "Camera.h"
#include "TextureLoader.h"

#include <iostream>

// Include GLEW
#include <GL/glew.h>

using namespace glm;

Skybox::Skybox() : Model() {

	//Create the Cube for the Skybox, as an array of floats
	float cubeVerticies[] = {
		-100.0f,  100.0f, -100.0f,
		-100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,

		-100.0f, -100.0f,  100.0f,
		-100.0f, -100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f, -100.0f,
		-100.0f,  100.0f,  100.0f,
		-100.0f, -100.0f,  100.0f,

		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,

		-100.0f, -100.0f,  100.0f,
		-100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f, -100.0f,  100.0f,
		-100.0f, -100.0f,  100.0f,

		-100.0f,  100.0f, -100.0f,
		100.0f,  100.0f, -100.0f,
		100.0f,  100.0f,  100.0f,
		100.0f,  100.0f,  100.0f,
		-100.0f,  100.0f,  100.0f,
		-100.0f,  100.0f, -100.0f,

		-100.0f, -100.0f, -100.0f,
		-100.0f, -100.0f,  100.0f,
		100.0f, -100.0f, -100.0f,
		100.0f, -100.0f, -100.0f,
		-100.0f, -100.0f,  100.0f,
		100.0f, -100.0f,  100.0f
	};

	//Create a vertex array
	glGenVertexArrays(1, &mSkyBoxVertexArrayID);

	//Generate and bind the vertex buffer
	glGenBuffers(1, &mSkyBoxVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mSkyBoxVertexBufferID);
	//Each triangle (3) * (6 sides of a cube * (2 triangles each * 3 verts)) * the size of a float
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), cubeVerticies, GL_STATIC_DRAW);

	//Load textures, and manually set the skyBoxID
	skyTextureID = 1;
	cubeMap("../Assets/Textures/negz.png", "../Assets/Textures/posz.png", "../Assets/Textures/negy.png", "../Assets/Textures/posy.png",
		"../Assets/Textures/negx.png", "../Assets/Textures/posx.png", &skyTextureID);

}

Skybox::~Skybox()
{
	//Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mSkyBoxVertexBufferID);
	glDeleteVertexArrays(1, &mSkyBoxVertexArrayID);
}

void Skybox::Update(float dt)
{
	Model::Update(dt);
}

void Skybox::Draw() {

	Renderer::CheckForErrors();

	//Set current shader to be the Textured Shader
	//First save the old shader
	ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();

	//Use the Skybox shaders
	Renderer::SetShader(ShaderType::SHADER_TEXTURE_SKY);
	glUseProgram(Renderer::GetShaderProgramID());

	Renderer::CheckForErrors();

	GLuint textureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "mySkyboxSamplerTexture");

	glUniform1i(textureLocation, 0);				// Set our Texture sampler to user Texture Unit 0

	Renderer::CheckForErrors();

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");

	// Send the view projection constants to the shader
	const Camera* currentCamera = World::GetInstance()->GetCurrentCamera();
	mat4 projectionM = currentCamera->GetProjectionMatrix();

	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &projectionM[0][0]);

	//Draw the vertex buffer
	//Generate and bind the vertex array
	glBindVertexArray(mSkyBoxVertexArrayID);

	//Use the Skybox shader
	glUseProgram(Renderer::GetShaderProgramID());

	//Check if the camera has moved, and if so, build a view matrix that contains no translation information
	//Pass this view Matrix to the Skybox shader
	if (Camera::hasMoved) {

		//Remove translation info from the View matrix
		mat4 viewMatrix = mat4(mat3(currentCamera->GetViewMatrix()));

		//Get the view transform matrix
		GLuint ViewMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");

		glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}
	else {

		mat4 viewMatrix = currentCamera->GetViewMatrix();

		//Get the view transform matrix
		GLuint ViewMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");

		glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mSkyBoxVertexBufferID);
	glVertexAttribPointer(0,			// attribute
		3,				// size
		GL_FLOAT,		// type
		GL_FALSE,		// normalized?
		0,				// stride
		nullptr         // array buffer offset
	);

	//Draw the cube!
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)

	glDisableVertexAttribArray(0);

	Renderer::CheckForErrors();

	//Set the old shader back
	Renderer::SetShader(oldShader);
	glUseProgram(Renderer::GetShaderProgramID());

}

void Skybox::cubeMap(const char* front, const char* back, const char* top, const char* bottom,
	const char* left, const char* right, GLuint* cubeTexture) {

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, cubeTexture);

	//Load each side of the cube (6 textures total) according to the their position
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	cubeMapSideLoader(*cubeTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);

}

bool Skybox::cubeMapSideLoader(GLuint skyTexture, GLenum side, const char* fileName) {

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);

	//Load image
	unsigned int image = TextureLoader::LoadSkyboxTexture(fileName, skyTexture, side);

	//Check that the image was loaded properly 
	if (image == 0) {
		fprintf(stderr, "ERROR: could not load %s\n", fileName);
		return false;
	}

	return true;
}

bool Skybox::ParseLine(const std::vector<ci_string> &token)
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