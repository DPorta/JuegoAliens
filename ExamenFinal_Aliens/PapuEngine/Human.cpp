#include "Human.h"
#include <random>
#include <ctime>
#include <glm/gtx/rotate_vector.hpp>

Human::Human()
{
}

void Human::init(float speed, glm::vec2 position)
{
	_speed = speed;
	_position = position;
	_color.set(200, 200, 0, 255);

	_direction = glm::vec2(0.0f, -1.0f);
}

void Human::update(const std::vector<std::string>& levelData,
	std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {

	collideWithLevel(levelData);
	_position += _direction * _speed;

}

Human::~Human()
{
}
