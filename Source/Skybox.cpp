#pragma once

#include "Skybox.h"
#include "ParsingHelper.h"
#include <GL/glew.h>
#include "Renderer.h"
#include "World.h"
#include "Camera.h"
#include <FreeImageIO.h>

Skybox::Skybox(vec3 size, string positiveXImagePath, string negativeXImagePath, string positiveYImagePath, string negativeYImagePath, string positiveZImagePath, string negativeZImagePath)
{
	skyboxFileNames[0] = positiveXImagePath;
	skyboxFileNames[1] = negativeXImagePath;
	skyboxFileNames[2] = positiveYImagePath;
	skyboxFileNames[3] = negativeYImagePath;
	skyboxFileNames[4] = positiveZImagePath;
	skyboxFileNames[5] = negativeZImagePath;

#if defined(PLATFORM_OSX)
	this->skyboxPositiveXID = TextureLoader::LoadTexture("Textures/mercury_rt.tga");
	this->skyboxNegativeXID = TextureLoader::LoadTexture("Textures/mercury_lf.tga");
	this->skyboxPositiveYID = TextureLoader::LoadTexture("Textures/mercury_up.tga");
	this->skyboxNegativeYID = TextureLoader::LoadTexture("Textures/mercury_dn.tga");
	this->skyboxPositiveZID = TextureLoader::LoadTexture("Textures/mercury_bk.tga");
	this->skyboxNegativeZID = TextureLoader::LoadTexture("Textures/mercury_ft.tga");
#else
	this->skyboxPositiveXID = TextureLoader::LoadTexture("../Assets/Textures/mercury_rt.tga");
	this->skyboxNegativeXID = TextureLoader::LoadTexture("../Assets/Textures/mercury_lf.tga");
	this->skyboxPositiveYID = TextureLoader::LoadTexture("../Assets/Textures/mercury_up.tga");
	this->skyboxNegativeYID = TextureLoader::LoadTexture("../Assets/Textures/mercury_dn.tga");
	this->skyboxPositiveZID = TextureLoader::LoadTexture("../Assets/Textures/mercury_bk.tga");
	this->skyboxNegativeZID = TextureLoader::LoadTexture("../Assets/Textures/mercury_ft.tga");
#endif

	for (int i = 0; i < 6; ++i) {
		// Load image using the Free Image library
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(skyboxFileNames[i].c_str(), 0);
		FIBITMAP* image = FreeImage_Load(format, skyboxFileNames[i].c_str());
		FIBITMAP* image32bits = FreeImage_ConvertTo32Bits(image);

		int width = FreeImage_GetWidth(image32bits);
		int height = FreeImage_GetHeight(image32bits);

		// This will upload the texture to the GPU memory
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(image32bits));

		// Free images
		FreeImage_Unload(image);
		FreeImage_Unload(image32bits);
	}

	vec3 halfSize = size * 0.5f;

	SkyboxVertex skyboxVertexBuffer[] = {
		// NegativeX Face
		{ vec3(halfSize.x, halfSize.y, halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(halfSize.x, -halfSize.y, -halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(halfSize.x, halfSize.y, -halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(halfSize.x, halfSize.y, halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(halfSize.x, -halfSize.y, -halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(halfSize.x, -halfSize.y, halfSize.z), vec3(1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		// PositiveX Face
		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(-halfSize.x, halfSize.y, -halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		// PositiveY Face
		{ vec3(halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(-halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		// NegativeY Face
		{ vec3(halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, -1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		// PositiveZ Face
		{ vec3(halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(halfSize.x, halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, -halfSize.z), vec3(0.0f, 0.0f, -1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		// NegativeZ Face
		{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(-halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f) },
		{ vec3(halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },

		{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f) },
		{ vec3(halfSize.x, halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f) },
		{ vec3(halfSize.x, -halfSize.y, halfSize.z), vec3(0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f) },
	};

	glGenVertexArrays(1, &skyboxVertexArrayID); // Create a vertex array
	glGenBuffers(1, &skyboxVertexBufferID); // Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertexBuffer), skyboxVertexBuffer, GL_STATIC_DRAW);
}

Skybox::~Skybox() {
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &skyboxVertexBufferID);
	glDeleteVertexArrays(1, &skyboxVertexArrayID);

	this->skyboxVertexBuffer.resize(0);
}

void Skybox::Update(float deltaTime) {
}

void Skybox::Draw() {

	ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_CUBEMAP_TEXTURED);
	glUseProgram(Renderer::GetShaderProgramID());

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);

	GLint previousCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &previousCullFaceMode);
	GLint previousDepthFunctionMode;
	glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunctionMode);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	int skyboxTextureIDs[6] = { this->skyboxPositiveXID, this->skyboxNegativeXID, this->skyboxPositiveYID, this->skyboxNegativeYID, this->skyboxPositiveZID, this->skyboxNegativeZID };

	GLuint textureCube;
	glGenTextures(1, &textureCube);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, *skyboxTextureIDs);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureIDs[0]);
	for (int skyboxTexture : skyboxTextureIDs) {
		glBindTexture(GL_TEXTURE_2D, skyboxTexture);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


	GLuint textureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "cubemapTexture");
	glUniform1i(textureLocation, 0);
	const Camera* currentCamera = World::GetInstance()->GetCurrentCamera();
	
	mat4 V = glm::mat4(glm::mat3(currentCamera->GetViewMatrix()));
	mat4 P = currentCamera->GetProjectionMatrix();
	mat4 VP = P * V;
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Draw the Vertex Buffer
	// Note this draws a unit Cube
	// The Model View Projection transforms are computed in the Vertex Shader
	glBindVertexArray(skyboxVertexArrayID);

	// 1st attribute buffer : vertex Positions
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBufferID);
	glVertexAttribPointer(0,				// attribute. No particular reason for 0, but must match the layout in the shader.
		3,				// size
		GL_FLOAT,		// type
		GL_FALSE,		// normalized?
		sizeof(SkyboxVertex), // stride
		(void*)0        // array buffer offset
	);

	

	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	glDisableVertexAttribArray(0);

	glCullFace(previousCullFaceMode);
	glDepthFunc(previousDepthFunctionMode);
	glDisable(GL_TEXTURE_CUBE_MAP);

	Renderer::SetShader(oldShader);


}