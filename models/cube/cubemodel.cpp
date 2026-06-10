#include "cubemodel.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../graphics/stb_image.h"
#include "../../graphics/shader.h"
#include "../../physics/physics.h"
#include "../../physics/physicsobject.h"
#include <iostream>

GLfloat TheCubeVertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// a simple cube init, but it send the vertices to the VBO
// unfortunatly it does this for every instance of a cube, which might be wasteful

CubeModel::CubeModel(Physics* phys)
	:ObjectModel(phys)
{

	mTranslationMatrix = glm::mat4(1.0f);
	mRotationMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);  // give the model 1 as an initialisation

	Scales = glm::vec3(1.0f, 1.0f, 1.0f);
	SetModelMatrix();
	this->Vertices = TheCubeVertices;
	Colour = glm::vec4(Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TheCubeVertices), TheCubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


}


// this is a rather pointless system, hard coding the file here? // TODO: remove double textures
CubeModel::CubeModel(const char* path, Physics* phys)
	:CubeModel(phys)
{
	//char* path = "/home/pi/projects/Pitfall3D/resources/testing/container2.png";
	glGenTextures(1, &texture1);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1) { format = GL_RED; }
		else if (nrComponents == 3) { format = GL_RGB; }
		else if (nrComponents == 4) { format = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		std::cout << "Because: " << stbi_failure_reason() << std::endl;
		stbi_image_free(data);
	}
}


CubeModel::~CubeModel()
{}




bool CubeModel::Update(const glm::mat4 view, const glm::mat4 projection)
{
	// Set up model view
	TheShader->use();

	// Get the transform from the RigidBody
	glm::mat4 transform = glm::mat4(1.0f);
	myPhysObj->GetTransform(glm::value_ptr(transform));

	// Create Scale Matrix
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);

	// Set new world position.
	btTransform btTransform = myPhysObj->GetRigidBody()->getWorldTransform();
	const glm::vec3 newWorldPos(btTransform.getOrigin().getX(), btTransform.getOrigin().getY() + 0.5f, btTransform.getOrigin().getZ());
	this->SetPosition(newWorldPos);
	MakeTranslationMatrix();

	Model = mTranslationMatrix * transform * mScaleMatrix;

	// Shader
	glEnable(GL_DEPTH_TEST);

	// Send our transformation to the currently bound shader, 
	TheShader->setMat4("model", Model);
	TheShader->setMat4("view", view);
	TheShader->setMat4("projection", projection);

	return true;
}
bool CubeModel::Draw()
{
	// Use the program object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	TheShader->use();

	//load the vertex data
	glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	// Load the texture coordinate
	glVertexAttribPointer(this->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLfloat*)(6 * sizeof(GLfloat)));

	// Load the normals
	glVertexAttribPointer(glGetAttribLocation(this->TheShader->ID, "aNormal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLfloat*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(this->positionLoc);
	glEnableVertexAttribArray(this->texCoordLoc);
	glEnableVertexAttribArray(glGetAttribLocation(this->TheShader->ID, "aNormal"));

	glBindTexture(GL_TEXTURE_2D, this->texture1);
	//glUniform1i(this->samplerLoc, 0);
	TheShader->setInt("ourTexture", 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(this->positionLoc);
	glDisableVertexAttribArray(this->texCoordLoc);
	glDisableVertexAttribArray(glGetAttribLocation(this->TheShader->ID, "aNormal"));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//std::cout << glGetError() << std::endl;
	if (glGetError() != GL_NO_ERROR)
	{
		printf("CubeModel Draw error\n");
		return false;
	}
	return true;
}


