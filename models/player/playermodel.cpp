#include "playermodel.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../graphics/shader.h"
#include "../../physics/physics.h"
#include "../../physics/physicsobject.h"

PlayerModel::PlayerModel(Physics* phys)
	:ObjectModel(phys)
{
	// a default system
	mTranslationMatrix = glm::mat4(1.0f);
	mRotationMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);  // give the model 1 as an initialisation
	const glm::vec3 pos = glm::vec3(11.0f, 0.0f, 0.4f);
	this->SetPosition(pos);

	Scales = glm::vec3(1.0f, 1.0f, 1.0f);
	SetModelMatrix();
	DegreeRotations = glm::vec3(0, 0, 0);
	Colour = glm::vec4(Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f);
}

// this is a handler to cope with OBJ files
PlayerModel::PlayerModel(char* fileName, Physics* phys)
	:PlayerModel(phys)
{
	// because we decided to have an OBJ LoadandConvert method in the base ObjectModel class, we can use that base class method to do our work for us
	// keeping this derived class simple
	const bool ret = ObjectModel::LoadAndConvert(&gdrawObjects, materials, textures, fileName);

	if (ret == false)
	{
		printf("This load for %s failed, check file name and try again!\n", fileName);
	}
}

PlayerModel::~PlayerModel()
{}

//float roll3 = 1.0f;

bool PlayerModel::Update(const glm::mat4 view, const glm::mat4 projection)
{
	// Set up model view
	TheShader->use();

	// Get the transform from the RigidBody
	glm::mat4 transform = glm::mat4(1.0f);
	myPhysObj->GetTransform(value_ptr(transform));

	// Create Scale Matrix
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);

	// Create Translation offset
	const glm::vec3 pos = glm::vec3(0.0f, -1.95f, 0.0f);
	MakeTranslationMatrix(pos);

	// Set new world position.
	const glm::vec3 newWorldPos(myPhysObj->GetRigidBody()->getCenterOfMassPosition().getX(),
		myPhysObj->GetRigidBody()->getCenterOfMassPosition().getY(), myPhysObj->GetRigidBody()->getCenterOfMassPosition().getZ());
	this->SetPosition(newWorldPos);

	// Make Model Matrix
	Model = mTranslationMatrix * transform * mScaleMatrix;


	// Shader
	glEnable(GL_DEPTH_TEST);

	// Make the MVP 
	glm::mat4 MVP = projection * view * Model;  // Remember order seems backwards

	glm::vec4 Amb = { 1.0f, 1.0f, 1.0f, 1.0f };
	TheShader->setVec4("Ambient", Amb);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	TheShader->setMat4("MVP", MVP);

	return true;
}

bool PlayerModel::Draw()
{
	TheShader->use();
	for (unsigned int frames = 0; frames < gdrawObjects.size(); frames++)
	{
		const DrawObject o = gdrawObjects[frames];
		glBindBuffer(GL_ARRAY_BUFFER, o.vb); 

		constexpr GLsizei stride = (3 + 3 + 3 + 2) * sizeof(GLfloat);
		//load the vertex data info
		glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);

		// Load the texture coordinate
		glVertexAttribPointer(this->texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 9));

		glEnableVertexAttribArray(this->positionLoc);
		glEnableVertexAttribArray(this->texCoordLoc);

		// bind the correct texture for this shape	
		if ((o.material_id < materials.size())) {
			std::string diffuse_texname = materials[o.material_id].diffuse_texname;
			if (textures.find(diffuse_texname) != textures.end()) {
				glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
			}
		}

		glUniform1i(this->samplerLoc, 0);
		glDrawArrays(GL_TRIANGLES, 0, o.numTriangles);
		if (glGetError() != GL_NO_ERROR)
		{
			printf("Oh bugger Player Model Draw error 1\n");
			return false;
		}

		// take these off the bind		
		glDisableVertexAttribArray(this->positionLoc);
		glDisableVertexAttribArray(this->texCoordLoc);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (glGetError() != GL_NO_ERROR)
		{
			printf("Oh bugger Player Model Draw error 2\n");
			return false;
		}

	}
	// always good to unbind	
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}