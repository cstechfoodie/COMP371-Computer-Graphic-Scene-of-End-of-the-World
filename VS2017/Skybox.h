#pragma once

#include "Model.h"
#include <GL/glew.h>

class Skybox : public Model {

private:
	unsigned int mSkyBoxVertexArrayID;
	unsigned int mSkyBoxVertexBufferID;
	GLuint skyTextureID;

public:
	Skybox();
	~Skybox();

	virtual void Update(float dt);
	virtual void Draw();

	void cubeMap(const char* front, const char* back, const char* top, const char* bottom,
		const char* left, const char* right, GLuint* cubeTexture);	//Creates the cube texture
	bool cubeMapSideLoader(GLuint texture, GLenum side, const char* fileName); //Loads the texture for each side of the cube

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

};