#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Spritesheet.h"


class Projectile {
public:
	Projectile(float, float, float, GLuint, float, float, float, float);

	void draw();
	void setVis(bool);
	void updatePos(float);
	float getY();
	float getX();
private:
	float x;
	float y;
	float speed;
	bool visible;
	
	GLuint textureID;

	Spritesheet sprite;
};