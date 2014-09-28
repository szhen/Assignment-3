#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include <time.h>

#include "general_functions.h"
#include "Projectile.h"
#include "entity.h"

#define MAX_ONSCREEN 26
#define MAX_PIKACHU 37
#define PIKACHU_SPEED 0.5 // change this value to change difficulty
#define PI 3.14159265358979323846

class Application {
public:
	Application();
	~Application();

	void Init();
	bool updateAndRender();
	void render();
	void update(float);
private:
	SDL_Window* displayWindow;

	const Uint8 *keys;
	
	SDL_Event event;

	bool start;
	
	float lastFrameTicks;
	float mainMenuFPS;
	float mainMenuElapsed;
	float shootCD;
	float lightningboltCD;

	enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };
	enum gameCondition { PRE_GAME, WON, LOST, IN_PROGRESS};
	int gameCondition;
	int state;
	int playerScore;
	unsigned int mainMenuCurrentIndex;
	unsigned int projectileIndex;
	unsigned int lightningboltIndex;
	GLuint fontTextureID;
	GLuint ssTextureID;

	std::vector<Entity> mainMenuEntity1;
	std::vector<Entity> mainMenuEntity2;
	Entity* pikachuArray[MAX_PIKACHU];
	Projectile* proj[MAX_ONSCREEN];
	Projectile* lightningboltArray[MAX_ONSCREEN];

	Entity red;

	void RenderMainMenu();
	void RenderGameLevel();
	void RenderGameOver();
	void renderPikachu();
	void renderLightningbolt();
	void initializeGame();
	void shootProj();
};