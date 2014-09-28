#include "Projectile.h"

Projectile::Projectile(float x, float y, float speed, GLuint textureID, float spriteX, float spriteY, float spriteW, float spriteH) : x(x), y(y), speed(speed), visible(false) {
	// hard coded for this specific hw
	Spritesheet temp(textureID, spriteX / 128.0f, spriteY / 128.0f, spriteW / 128.0f, spriteH / 128.0f);
	sprite = temp;
}

void Projectile::draw() {
	if (!visible)
		return;
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	sprite.draw(0.1f);
}

void Projectile::setVis(bool var) {	visible = var; }

void Projectile::updatePos(float elapsed) {	y += speed * elapsed; }

float Projectile::getY() {	return y; }

float Projectile::getX() {	return x; }