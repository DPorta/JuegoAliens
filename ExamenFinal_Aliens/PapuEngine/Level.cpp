#include "Level.h"
#include <fstream>
#include "ResourceManager.h"
#include "Error.h"
#include <random>
#include <ctime>


Level::Level(const std::string& fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (file.fail()) {
		fatalError("failed to opem " + fileName);
	}
	std::string tmp;

	file >> tmp >> numHumans;

	std::getline(file, tmp);
	while (std::getline(file, tmp)) {
		levelData.push_back(tmp);
	}
	parseLevel();

}

void Level::draw() {
	spriteBatch.renderBatch();
}


void Level::parseLevel() {
	spriteBatch.init();
	spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	Color color;
	color.set(255, 255, 255, 255);
	for (int y = 0; y < levelData.size(); y++)
	{
		for (int x = 0; x < levelData[y].size(); x++)
		{
			char tile = levelData[y][x];
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH,
				TILE_WIDTH, TILE_WIDTH);
			switch (tile)
			{
			case 'R':
			case 'B':
				spriteBatch.draw(glm::vec4(-300,-50,1200,800), uvRect,
					ResourceManager::getTexture("Textures/fondos/game.png").id
					, 0.0f, color);
				break;
			case '@':
				levelData[y][x] = '.';
				playerPosition.x = x * TILE_WIDTH;
				playerPosition.y = y * TILE_WIDTH;
				break;
			default:
				break;
			}
		}
	}
	spriteBatch.end();
}


Level::~Level()
{
}