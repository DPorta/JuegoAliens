#include <iostream>
#include <random>
#include <ctime>
#include "MainGame.h"
#include "Sprite.h"
#include "ImageLoader.h"
#include "ResourceManager.h"
#include "PapuEngine.h"


using namespace std;

void MainGame::run() {
	init();
	update();
}

void MainGame::init() {
	Papu::init();
	_window.create("Engine", _witdh, _height, 0);
	initShaders();
	initLevel();

}

void MainGame::initLevel() {
	_levels.push_back(new Level("Levels/Nivel_1_de_jueguitos.txt"));
	_player = new Player();
	_currentLevel = 0;
	_player->init(0.3f,
		_levels[_currentLevel]->getPlayerPosition(), &_inputManager);
	_spriteBacth.init();

	for (size_t i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_zombies.push_back(new Zombie());
		glm::vec2 pos(-300 + rand() % (1200 - 1), 800 + rand() % (10000 - 1));
		_zombies.back()->init(0.2f, pos);
	}
	for (size_t i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_vidas.push_back(new Vidas());
		glm::vec2 pos(-300 + rand() % (1200 - 1), 800 + rand() % (10000 - 1));
		_vidas.back()->init(0.2f, pos);
	}
	for (size_t i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_humans.push_back(new Human());
		glm::vec2 pos(-300 + rand() % (1200 - 1), 800 + rand() % (10000 - 1));
		_humans.back()->init(0.2f, pos);

	}
}

void MainGame::initShaders() {
	_program.compileShaders("Shaders/colorShaderVert.txt",
		"Shaders/colorShaderFrag.txt");
	_program.addAtribute("vertexPosition");
	_program.addAtribute("vertexColor");
	_program.addAtribute("vertexUV");
	_program.linkShader();
}


void MainGame::draw() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_program.use();

	glActiveTexture(GL_TEXTURE0);

	GLuint pLocation =
		_program.getUniformLocation("P");

	glm::mat4 cameraMatrix = _camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	GLuint imageLocation = _program.getUniformLocation("myImage");
	glUniform1i(imageLocation, 0);

	_spriteBacth.begin();
	_levels[_currentLevel]->draw();


	for (size_t i = 0; i < _zombies.size(); i++)
	{
		_zombies[i]->draw(_spriteBacth);
	}
	for (size_t i = 0; i < _vidas.size(); i++)
	{
		_vidas[i]->draw(_spriteBacth);
	}
	for (size_t i = 0; i < _humans.size(); i++)
	{
		_humans[i]->draw(_spriteBacth);
	}

	_player->draw(_spriteBacth);

	_spriteBacth.end();
	_spriteBacth.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);

	_program.unuse();
	_window.swapBuffer();
}

void MainGame::procesInput() {
	SDL_Event event;
	const float CAMERA_SPEED = 20.0f;
	const float SCALE_SPEED = 0.1f;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords(event.motion.x, event.motion.y);
			break;
		case  SDL_KEYUP:
			_inputManager.releaseKey(event.key.keysym.sym);
			break;
		case  SDL_KEYDOWN:
			_inputManager.pressKey(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(event.button.button);
			break;
		}

		if (_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
			glm::vec2 mouseCoords = _camera.convertScreenToWorl(_inputManager.getMouseCoords());
			cout << mouseCoords.x << " " << mouseCoords.y << endl;
		}
	}
}

void MainGame::update() {
	int puntos = 0;
	while (_gameState != GameState::EXIT) {
		procesInput();
		draw();
		_camera.update();
		_time += 0.002f;
		_player->update(_levels[_currentLevel]->getLevelData(), _zombies);
		_camera.setPosition(glm::vec2(300.0f, 350.0f));

		for (size_t i = 0; i < _zombies.size(); i++)
		{
			_zombies[i]->update(_levels[_currentLevel]->getLevelData(), _zombies);

			if (_zombies[i]->collidedWithAgent(_player)&&_inputManager.isKeyPressed(SDLK_w))
			{
				puntos += 20;
				std::cout << "Ganas 20, tienes: " << puntos << endl;
				_zombies.erase(_zombies.begin() + i);
			}
			else if (_zombies[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_q)) {
				puntos -= 10;
				std::cout << "Pierdes 10, tienes: " << puntos << endl;
				_zombies.erase(_zombies.begin() + i);
			}
			else if (_zombies[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_e)) {
				puntos -= 5;
				std::cout << "Pierdes 5, tienes: " << puntos << endl;
				_zombies.erase(_zombies.begin() + i);
			}
			if (puntos < 0) {
				SDL_Quit();
				SDL_CreateWindow("Me fui a la bica papu y mamu", 300, SDL_WINDOWPOS_CENTERED, 500, 5, SDL_WINDOW_OPENGL);
				return;
			}
		}
		for (size_t i = 0; i < _vidas.size(); i++)
		{
			_vidas[i]->update(_levels[_currentLevel]->getLevelData(), _vidas);

			if (_vidas[i]->collidedWithAgent(_player)&&_inputManager.isKeyPressed(SDLK_e))
			{
				puntos *= 2;
				std::cout << "Duplicaste, ahora tienes: " << puntos << endl;
				_vidas.erase(_vidas.begin() + i);
			}
			else if (_vidas[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_q)) {
				puntos -= 20;
				std::cout << "Pierdes 20, tienes: " << puntos << endl;
				_vidas.erase(_vidas.begin() + i);
			}
			else if (_vidas[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_w)) {
				puntos -= 15;
				std::cout << "Pierdes 15, tienes: " << puntos << endl;
				_vidas.erase(_vidas.begin() + i);
			}
			if (puntos < 0) {
				SDL_Quit();
				SDL_CreateWindow("Me fui a la bica papu y mamu", 600, SDL_WINDOWPOS_CENTERED, 1000, 5, SDL_WINDOW_OPENGL);
				return;
			}
		}
		for (size_t i = 0; i < _humans.size(); i++)
		{
			_humans[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies);
			if (_humans[i]->collidedWithAgent(_player)&&_inputManager.isKeyPressed(SDLK_q)) {
				puntos += 10;
				std::cout << "Ganas 10, tienes: " << puntos << endl;
				_humans.erase(_humans.begin() + i);
			}
			else if (_humans[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_w)) {
				puntos -= 15;
				std::cout << "Pierdes 15, tienes: " << puntos << endl;
				_humans.erase(_humans.begin() + i);
			}
			else if(_humans[i]->collidedWithAgent(_player) && _inputManager.isKeyPressed(SDLK_e)){
				puntos /= 2;
				std::cout << "Pierdes la mitad, tienes: " << puntos << endl;
				_humans.erase(_humans.begin() + i);
			}
			if (puntos < 0) {
				SDL_Quit();
				SDL_CreateWindow("Me fui a la bica papu y mamu", 600, SDL_WINDOWPOS_CENTERED, 1000, 5, SDL_WINDOW_OPENGL);
				return;
			}
		}
	}
}


MainGame::MainGame() :
	_witdh(1200),
	_height(800),
	_gameState(GameState::PLAY),
	_time(0)
{
	_camera.init(_witdh, _height);
}


MainGame::~MainGame()
{
}
