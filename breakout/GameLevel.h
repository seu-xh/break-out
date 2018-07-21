#pragma once
#include<vector>
#include"Gameobject.h"
#include<sstream>
#include<fstream>

class GameLevel
{
public:
	std::vector<GameObject>Bricks;
	GameLevel() {};
	void Load(const char *file, GLuint levelwidth, GLuint levelheight);
	void Draw(SpriteRenderer &renderer);
	GLboolean IsCompleted();//Pass the level??

private:
	void init(std::vector<std::vector<GLuint>>tileData, GLuint levelwidth, GLuint levelheight);

};
