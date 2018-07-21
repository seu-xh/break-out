#include"GameLevel.h"

void GameLevel::Load(const char *file, GLuint levelwidth, GLuint levelheight)
{
	this->Bricks.clear();
	GLuint tilecode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>>tileData;
	if (fstream) {
		while (std::getline(fstream, line)) {
			std::istringstream sstream(line);
			std::vector<GLuint>row;
			while (sstream >> tilecode)
				row.push_back(tilecode);
			tileData.push_back(row);
		}
		if (!tileData.empty()) {
			this->init(tileData, levelwidth, levelheight);
		}
	}
};
void GameLevel::Draw(SpriteRenderer &renderer) {
	for (GameObject &tile : this->Bricks)
		if (!tile.Destroyed)
			tile.Draw(renderer);
}
GLboolean GameLevel::IsCompleted()
{
	for (GameObject &tile : this->Bricks)
		if (!tile.Destroyed)
			return GL_FALSE;
	return GL_TRUE;
}


void GameLevel::init(std::vector<std::vector<GLuint>>tileData, GLuint levelwidth, GLuint levelheight)
{
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
	GLuint unit_width = levelwidth / static_cast<GLfloat>(width);
	GLuint unit_height = levelheight / static_cast<GLfloat>(height);
	for (GLuint y = 0; y < height; y++)
	{
		for (GLuint x = 0; x < width; x++)
		{
			if (tileData[y][x] == 1)
			{
				glm::vec2 pos(unit_width*x, unit_height*y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = GL_TRUE;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)
			{
				glm::vec3 color = glm::vec3(1.0f);
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);
				glm::vec2 pos(unit_width*x, unit_height*y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block"), color);
				this->Bricks.push_back(obj);
			}
		}
	}
}
