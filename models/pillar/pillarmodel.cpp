#include "pillarmodel.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../graphics/shader.h"
#include "../../physics/physics.h"
#include "../../physics/physicsobject.h"
#include "../../graphics/stb_image.h"

PillarModel::PillarModel(Physics* phys)
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
PillarModel::PillarModel(char* fileName, Physics* phys)
	:PillarModel(phys)
{
	// because we decided to have an OBJ LoadandConvert method in the base ObjectModel class, we can use that base class method to do our work for us
	// keeping this derived class simple
	const bool ret = ObjectModel::LoadAndConvert(&gdrawObjects, materials, textures, fileName);

	if (ret == false)
	{
		printf("This load for %s failed, check file name and try again!\n", fileName);
	}

	glGenTextures(1, &texture2);
	const char* path = "../../../resources/testing/lightmap.png";
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1) { format = GL_RED; }
		else if (nrComponents == 3) { format = GL_RGB; }
		else if (nrComponents == 4) { format = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, texture2);
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

PillarModel::~PillarModel()
{}

bool PillarModel::Update(const glm::mat4 view, const glm::mat4 projection)
{
	// Set up model view
	TheShader->use();
	TheShader->setVec3("lightColor", 0.9f, 0.9f, 0.9f);

	// Get the transform from the RigidBody
	glm::mat4 transform;
	myPhysObj->GetTransform(value_ptr(transform));

	// Create Scale Matrix
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);

	// Set new world position.
	btTransform btTransform = myPhysObj->GetRigidBody()->getWorldTransform();
	const glm::vec3 newWorldPos(btTransform.getOrigin().getX(), btTransform.getOrigin().getY(), btTransform.getOrigin().getZ());
	this->SetPosition(newWorldPos);
	MakeTranslationMatrix();

	MakeRotationMatrix();

	// Make Model Matrix
	Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;

	// Shader
	glEnable(GL_DEPTH_TEST);

	// Send our transformation to the currently bound shader
	TheShader->setMat4("model", Model);
	TheShader->setMat4("view", view);
	TheShader->setMat4("projection", projection);

	return true;
}

bool PillarModel::Draw()
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

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->texture2);

		glActiveTexture(GL_TEXTURE0);

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
			printf("Oh bugger Pillar Model Draw error 1\n");
			return false;
		}

		// take these off the bind		
		glDisableVertexAttribArray(this->positionLoc);
		glDisableVertexAttribArray(this->texCoordLoc);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (glGetError() != GL_NO_ERROR)
		{
			printf("Oh bugger Pillar Model Draw error 2\n");
			return false;
		}

	}
	// always good to unbind	
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}