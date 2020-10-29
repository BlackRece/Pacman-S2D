#include "Munchie.h"

Munchie::Munchie(int munchieSize) {
	this->sourceRect = new Rect(0, 0, munchieSize, munchieSize);
	this->position = new Rect(0, 0, munchieSize, munchieSize);
	this->isEaten = false;
	this->isPowerPellet = false;
	this->value = 0;
	this->scale = 0;
	this->offset = 0;
}

Munchie::~Munchie() {
}

void Munchie::Draw(int frameCount) {
	if (!this->isEaten) {
		Entity::Draw(frameCount);
	}
}
