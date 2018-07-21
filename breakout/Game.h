#pragma once
#define GLEW_STATIC
#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include<vector>
#include"GameLevel.h"
#include"BallObject.h"
#include"parcticle_generator.h"
#include"PostProcessor.h"
#include<tuple>
#include"powerup.h"
enum Direction
{
	UP, RIGHT, DOWN,LEFT
};
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;
enum Gamestate
{
	GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
class Game
{
public:
	std::vector<GameLevel>Levels;//游戏关卡
	GLuint Level;//当前关卡数
	Gamestate State;
	GLboolean Keys[1024];
	GLuint Width, Height;
	std::vector<PowerUp>PowerUps;
	void SpawnPowerUps(GameObject&block);
	void UpdatePowerUps(GLfloat dt);
	Game(GLuint width,GLuint height);
	~Game();
	void ResetLevel();
	void ResetPlayer();
	void Init();
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();
	//GLboolean CheckCollision(GameObject &one, GameObject &two);
	Collision CheckCollision(GameObject &one, BallObject &two);
	GLboolean CheckCollision(GameObject &one, GameObject &two);
private:

};

