#include "physicsdraw.h"
#include "../graphics/shader.h"
#include <GLES3/gl3.h>

GLuint PhysicsDraw::ProgramObject = -1;

void PhysicsDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	// draws a simple line of pixels between points but stores them for later draw
	LineValues Line;
	Line.p1 = from;
	Line.Colour1 = color;
	Line.p2 = to;
	Line.Colour2 = color;

	TheLines.push_back(Line);
}

void PhysicsDraw::DoDebugDraw()
{// set up the line shader and then draw the buffer
			//load the vertex data info


	if (static_cast<int>(ProgramObject) == -1)
	{ // make the shader
		shader = new Shader((char*)"../../../resources/shaders/SimpleTriV.txt", (char*)"../../../resources/shaders/SimpleTriF.txt");

		ProgramObject = shader->ID;

		if (static_cast<int>(ProgramObject) == -1)
		{
			printf("wtf happened");
		}



	}
	if (TheLines.size() == 0) return;

	glUseProgram(ProgramObject);

	positionLoc = glGetAttribLocation(ProgramObject, "aPos");
	ColourLoc = glGetAttribLocation(ProgramObject, "aColor");

	constexpr int stride = 8 * sizeof(float); // 4 floats for each btvec3

	glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, stride, (GLfloat*)&TheLines[0]);

	GLuint LineError = glGetError();
	if (LineError != GL_NO_ERROR) { std::cout << "Oh bugger Line Draw error 1A: " << LineError << std::endl; }

	glVertexAttribPointer(this->ColourLoc, 3, GL_FLOAT, GL_FALSE, stride, (GLfloat*)&TheLines[0].Colour1);

	LineError = glGetError();
	if (LineError != GL_NO_ERROR) { std::cout << "Oh bugger Line Draw error 1B: " << LineError << std::endl; }

	glEnableVertexAttribArray(this->positionLoc);
	glEnableVertexAttribArray(this->ColourLoc);

	glDrawArrays(GL_LINES, 0, (TheLines.size() * 2)); // There are 2 components to send

	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(ColourLoc);

	LineError = glGetError();
	if (LineError != GL_NO_ERROR) { std::cout << "Oh bugger Line Draw error 2: " << LineError << std::endl; }

	TheLines.clear();
}
void PhysicsDraw::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	drawLine(pointOnB, pointOnB + (normalOnB * 2), color);
}


void PhysicsDraw::ToggleDebugFlag(const int flag)
{
	// checks if a flag is set and enables/
	// disables it
	if (m_debugMode & flag)
		// flag is enabled, so disable it
		m_debugMode = m_debugMode & (~flag);
	else
		// flag is disabled, so enable it
		m_debugMode |= flag;
}
