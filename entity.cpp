#include "entity.h"

Entity::Entity(float x, float y, float rot, GLuint textID, float w, float h) : x(x), y(y), rotation(rot), 
textureID(textID), width(w), height(h) {}

Entity::Entity(Spritesheet temp, float x, float y) {
	sprite = temp;
	Entity::x = x;
	Entity::y = y;
	rotation = 0.0f;
	textureID = 0;
	width = temp.getWidth();
	height = temp.getHeight();
	direction = direction::RIGHT;
}

Entity::Entity() {
	x = 0.0f;
	y = 0.0f;
	rotation = 0.0f;

	textureID = 0;

	width = 0.0f;
	height = 0.0f;
}

void Entity::setX(float newX) {	x = newX; }

void Entity::setY(float newY) {	y = newY; }

void Entity::setDirection(int dire) { direction = dire; }

float Entity::getX() { return x; }

float Entity::getY() { return y; }

int Entity::getDirection() { return direction; }

void Entity::Draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -width * 0.5f, height * 0.5f, -width * 0.5f, -height * 0.5f, width * 0.5f, -height * 0.5f, width * 0.5f, height * 0.5f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void Entity::ssDraw(float scale) {
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);
	sprite.draw(scale);
}

void Entity::setSprite(Spritesheet temp) { sprite = temp; }