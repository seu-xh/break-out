#pragma once
#define GLEW_STATIC
#include<GL\glew.h>
#include<glm\glm.hpp>
#include"Texture.h"
#include"Shader.h"
#include"Gameobject.h"
#include<vector>

struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	float life;
	glm::vec4 color;
	Particle():position(0.0f),velocity(0.0f),life(0.0f),color(1.0f){}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Texture2D texture,Shader shader,GLuint amount);
	void Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f));
	void Draw();
private:
	std::vector<Particle>particles;
	GLuint amount;
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	void init();
	GLuint firstUnusedParticle();
	void RespawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f));

};



