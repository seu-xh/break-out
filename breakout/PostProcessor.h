#pragma once
#define GLEW_STATIC
#include<GL/glew.h>
#include<glm/glm.hpp>

#include"Texture.h"
#include"Shader.h"
#include"SpriteRenderer.h"

class PostProcessor
{
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	GLuint Width, Height;
	GLboolean Confuse, Chaos, Shake;
	PostProcessor(Shader shader,GLuint width,GLuint height);
	void BeginRender();
	void EndRender();
	void Render(GLfloat time);
private:
	GLuint MSFBO, FBO;
	GLuint RBO;
	GLuint VAO;

	void initRenderData();
};

