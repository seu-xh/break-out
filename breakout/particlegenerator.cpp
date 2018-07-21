#include"parcticle_generator.h"

ParticleGenerator::ParticleGenerator(Texture2D texture, Shader shader, GLuint amount) :texture(texture), shader(shader), amount(amount)
{
	this->init();
}

void ParticleGenerator::init()
{
	for (int i = 0; i < this->amount; i++)
	{
		this->particles.push_back(Particle());
	}
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset)
{
	for (int i = 0; i < newParticles; i++)
	{
		GLuint tmp = this->firstUnusedParticle();
		this->RespawnParticle(this->particles[tmp], object, offset);
	}
	for (Particle &particle : this->particles)
	{
		particle.life -= dt;
		if (particle.life > 0.0f)
		{
			particle.position -= particle.velocity*dt;
			particle.color.a -= 2.5*dt;
		}
	}
}

void ParticleGenerator::Draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();
	for (Particle &particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			this->shader.SetVector2f("offset", particle.position);
			this->shader.SetVector4f("color", particle.color);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint lastUnused = 0;

GLuint ParticleGenerator::firstUnusedParticle()
{
	for (int i = lastUnused; i < this->amount; i++)
	{
		if (this->particles[i].life < 0.0f) {
			lastUnused = i;
			return i;
		}
			
   }
	for (int i = 0; i < lastUnused; i++)
	{
		if (this->particles[i].life < 0.0f) {
			lastUnused = i;
			return i;
		}
	}
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.position = object.Position + random + offset;
	particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.life = 1.0f;
	particle.velocity = object.Velocity*0.1f;
}