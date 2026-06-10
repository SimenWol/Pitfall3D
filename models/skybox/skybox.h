#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Shader;

class SkyBox
{
public:
	SkyBox(Shader* shaderIn);
	~SkyBox();

	void Draw(glm::mat4 view, glm::mat4 projection) const;
private:
	unsigned int loadTextures(std::vector<std::string> faces);
private:
	Shader* shader;
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;

    std::vector<std::string> faces
    {
        "../../../resources/skybox/right.jpg",
        "../../../resources/skybox/left.jpg",
        "../../../resources/skybox/top.jpg",
        "../../../resources/skybox/bottom.jpg",
        "../../../resources/skybox/front.jpg",
        "../../../resources/skybox/back.jpg"
    };
};