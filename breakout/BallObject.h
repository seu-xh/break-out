#pragma once
#include"Gameobject.h"
class BallObject:public GameObject
{
public:
	GLfloat Radius;
	GLboolean Stuck;//初始状态随paddle一起移动
	GLboolean Sticky=false;
	GLboolean PassThrough=false;
	BallObject();
	BallObject(glm::vec2 pos,GLfloat radius,glm::vec2 velocity,Texture2D sprite);
	~BallObject();
	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
private:

};

