#pragma once //for header files

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "general_functions.h"
#include "Spritesheet.h"

#define PI 3.14159265358979323846

class Entity {
public:
	Entity(float, float, float, GLuint, float, float);
	Entity(Spritesheet, float, float);
	Entity();

	void Draw();
	void ssDraw(float);

	void setSprite(Spritesheet);
	void setY(float);
	void setX(float);
	void setDirection(int);
	float getX();
	float getY();
	int getDirection();
private:
	int direction;
	float x;
	float y;
	float rotation;

	GLuint textureID;

	float width;
	float height;

	Spritesheet sprite;
};