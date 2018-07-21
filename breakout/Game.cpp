#include"Game.h"
#include<algorithm>
//#include"SpriteRenderer.h"
SpriteRenderer *Renderer;
GameObject *Player;
BallObject *ball;
ParticleGenerator *particle;
PostProcessor *effect;
Game::Game(GLuint width, GLuint height) :State(GAME_ACTIVE), Keys(), Width(width), Height(height) 
{

}

Game::~Game() {
	delete Renderer;
	delete Player;
}

const glm::vec2 INITIAL_VELOCITY(100.0f, -350.0f);
const GLfloat RADIUS(12.5f);
const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);
GLfloat ShakeTime = 0.0f;
void Game::Init()
{
	ResourceManager::LoadShader("shader/vs.txt", "shader/fs.txt", nullptr, "sprite");
	ResourceManager::LoadShader("shader/particlevs.txt", "shader/particlefs.txt", nullptr, "particlesprite");
	
	ResourceManager::LoadShader("shader/postprocessvs.txt", "shader/postprocessfs.txt", nullptr, "effect");
	
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("particlesprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("particlesprite").SetMatrix4("projection", projection);

	Renderer= new SpriteRenderer(ResourceManager::GetShader("sprite"));
	ResourceManager::LoadTexture("textures/container.jpg", GL_FALSE, "container");
	ResourceManager::LoadTexture("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("textures/particle.png", GL_TRUE, "particle");
	ResourceManager::LoadTexture("textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::LoadTexture("textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
	// 加载关卡
	GameLevel one; one.Load("levels/one.txt", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("levels/two.txt", this->Width, this->Height * 0.5);
	GameLevel three; three.Load("levels/three.txt", this->Width, this->Height * 0.5);
	GameLevel four; four.Load("levels/four.txt", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	particle = new ParticleGenerator(ResourceManager::GetTexture("particle"), ResourceManager::GetShader("particlesprite"), 1000);
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x, this->Height - PLAYER_SIZE.y);
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - RADIUS, -RADIUS*2);
	Player = new GameObject(playerPos,PLAYER_SIZE,ResourceManager::GetTexture("paddle"));
	ball = new BallObject(ballPos, RADIUS, INITIAL_VELOCITY, ResourceManager::GetTexture("face"));
	effect = new PostProcessor(ResourceManager::GetShader("effect"), this->Width, this->Height);
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::UpdatePowerUps(GLfloat dt)
{
	for (PowerUp &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// Remove powerup from list (will later be removed)
				powerUp.Activated = GL_FALSE;
				// Deactivate effects
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{	// Only reset if no other PowerUp of type sticky is active
						ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	// Only reset if no other PowerUp of type pass-through is active
						ball->PassThrough = GL_FALSE;
						ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{	// Only reset if no other PowerUp of type confuse is active
						effect->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{	// Only reset if no other PowerUp of type chaos is active
						effect->Chaos = GL_FALSE;
					}
				}
			}
		}
	}
	// Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
	// Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

GLboolean ShouldSpawn(GLuint chance) {
	GLuint random = rand() % chance;
	return random == 0;
}
void Game::SpawnPowerUps(GameObject&block) {
	if (ShouldSpawn(75)) // 1 in 75 chance
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // Negative powerups should spawn more often
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void ActivatePowerUp(PowerUp &powerUp)
{
	// Initiate a powerup based type of powerup
	if (powerUp.Type == "speed")
	{
		ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		ball->PassThrough = GL_TRUE;
		ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!effect->Chaos)
			effect->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "chaos")
	{
		if (!effect->Confuse)
			effect->Chaos = GL_TRUE;
	}
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	// Check if another PowerUp of the same type is still active
	// in which case we don't disable its effect (yet)
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}

void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (this->Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				if (ball->Stuck)
					ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (ball->Stuck)
					ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			ball->Stuck = false;
	}
}
void Game::ResetLevel() {
	if (this->Level == 0)this->Levels[0].Load("levels/one.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("levels/two.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("levels/three.txt", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("levels/four.txt", this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer() {
	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x, this->Height - PLAYER_SIZE.y);
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - RADIUS, -RADIUS * 2);
	Player->Position = playerPos;
	ball->Reset(ballPos, INITIAL_VELOCITY);
}
void Game::Update(GLfloat dt)
{
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime < 0.0f)
			effect->Shake = false;
	}
	ball->Move(dt, this->Width);
	DoCollisions();
	if (ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
	particle->Update(dt, *ball, 5, glm::vec2(ball->Radius / 2));
	UpdatePowerUps(dt);
}

void Game::Render()
{

if (this->State == GAME_ACTIVE) {
	effect->BeginRender();
	Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
	this->Levels[this->Level].Draw(*Renderer);
	Player->Draw(*Renderer);
	ball->Draw(*Renderer);
	// Draw PowerUps
	for (PowerUp &powerUp : this->PowerUps)
		if (!powerUp.Destroyed)
			powerUp.Draw(*Renderer);
	particle->Draw();
	effect->EndRender();
	effect->Render(glfwGetTime());
}
}
Direction VirDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f)
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(target, compass[i]);
		if (dot_product>max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;

}

GLboolean Game::CheckCollision(GameObject &one, GameObject &two)
{
	GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x&&two.Position.x + two.Size.x >= one.Position.x;
	GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y &&two.Position.y + two.Size.y >= one.Position.y;
	return collisionX && collisionY;
}

Collision Game::CheckCollision(GameObject &one,BallObject &two)
{
	glm::vec2 center(two.Position + two.Radius);
	glm::vec2 aabb_half_extents(one.Size.x / 2,one.Size.y/2);
	glm::vec2 aabb_center(one.Position + glm::vec2(one.Size.x / 2, one.Size.y / 2));
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = center - closest;
	if(glm::length(difference) < two.Radius)
		return std::make_tuple(GL_TRUE,VirDirection(difference),difference);
	else return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}
void Game::DoCollisions()
{
	for (GameObject &box : this->Levels[this->Level].Bricks) {
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(box, *ball);
			if (std::get<0>(collision))
			{
				if (!box.IsSolid)
				{
					box.Destroyed = GL_TRUE;
					SpawnPowerUps(box);
				}
				else {
					ShakeTime = 0.05f;
					effect->Shake = true;
				}
				Direction direction = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(ball->PassThrough && !box.IsSolid)) {
				if (direction == LEFT || direction == RIGHT)
				{
					ball->Velocity.x = -ball->Velocity.x;
					GLfloat penetration = ball->Radius - std::abs(diff_vector.x);
					if (direction == LEFT)
						ball->Position.x += penetration;
					else
						ball->Position.x -= penetration;
				}
				else
				{
					ball->Velocity.y = -ball->Velocity.y; // 反转垂直速度
														  // 重定位
					GLfloat penetration = ball->Radius - std::abs(diff_vector.y);
					if (direction == UP)
						ball->Position.y -= penetration; // 将球上移
					else
						ball->Position.y += penetration; // 将球下移
				}
			}
			}
		}
	}

	for (PowerUp &powerUp : this->PowerUps)//道具
	{
		if (!powerUp.Destroyed)
		{
			// First check if powerup passed bottom edge, if so: keep as inactive and destroy
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = GL_TRUE;

			if (CheckCollision(*Player,powerUp))
			{	// Collided with player, now activate powerup
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
			}
		}
	}


	//挡板与球的碰撞
	Collision result = CheckCollision(*Player, *ball);
	if (!ball->Stuck&&std::get<0>(result))
	{
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (ball->Position.x + ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x/ 2);

		GLfloat strength = 2.0f;
		glm::vec2 OldVelocity = ball->Velocity;
		ball->Velocity.x = INITIAL_VELOCITY.x * percentage * strength;
		//ball->Velocity.y = -ball->Velocity.y;(粘板)
		ball->Velocity.y = -std::abs(ball->Velocity.y);
		ball->Velocity = glm::normalize(ball->Velocity)*glm::length(OldVelocity);
		// If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
		ball->Stuck = ball->Sticky;
	}
}