#include "shipmodel.h"
#include "../../graphics/shader.h"
#include "../../physics/physics.h"

ShipModel::ShipModel(Physics* phys)
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
ShipModel::ShipModel(char* fileName, Physics* phys)
	:ShipModel(phys)
{
	// because we decided to have an OBJ LoadandConvert method in the base ObjectModel class, we can use that base class method to do our work for us
	// keeping this derived class simple
	const bool ret = ObjectModel::LoadAndConvert(&gdrawObjects, materials, textures, fileName);

	if (ret == false)
	{
		printf("This load for %s failed, check file name and try again!\n", fileName);
	}
}

ShipModel::~ShipModel()
{}

bool ShipModel::Update(const glm::mat4 view, const glm::mat4 projection)
{
	//set up model view
	TheShader->use();
	DegreeRotations.x += 1.1f;
	Rotations.x = DEG2RAD(DegreeRotations.x);

	DegreeRotations.y -= 1.1f;
	Rotations.y = DEG2RAD(DegreeRotations.y);

	Rotations.z = DEG2RAD(DegreeRotations.z);
	mScaleMatrix = glm::mat4(1.0f);


	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	MakeRotationMatrix();
	MakeTranslationMatrix();
	MakeModelMatrix();

	TheShader->use();

	glEnable(GL_DEPTH_TEST);

	// make the MVP 
	const glm::mat4 MVP = projection * view * Model;  // Remember order seems backwards

	const glm::vec4 amb = { 1.0f, 1.0f, 1.0f, 1.0f }; 
	TheShader->setVec4("Ambient", amb);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	TheShader->setMat4("MVP", MVP);

	return true;
}

bool ShipModel::Draw()
{
	TheShader->use();
	for (unsigned int frames = 0; frames < gdrawObjects.size(); frames++)
	{
		const DrawObject o = gdrawObjects[frames];
		glBindBuffer(GL_ARRAY_BUFFER, o.vb);

		constexpr GLsizei stride = (3 + 3 + 3 + 2) * sizeof(GLfloat); 
		//load the vertex data info
		glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, stride, (const void*)nullptr);

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
			printf("Ship Model Draw error 1\n");
			return false;
		}

		// take these off the bind		
		glDisableVertexAttribArray(this->positionLoc);
		glDisableVertexAttribArray(this->texCoordLoc);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (glGetError() != GL_NO_ERROR)
		{
			printf("Ship Model Draw error 2\n");
			return false;
		}

	}
	// always good to unbind	
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}