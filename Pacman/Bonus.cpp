#include "Bonus.h"

Bonus::Bonus() {
	//this->position = new Rect();
	this->spawnMaxTime = 2000;	// maximum duration before timer resets
	this->spawnTimer = 0;		// current counter 
	this->spawnDelay = this->spawnMaxTime /2;		// ...
	this->isShown = false;
	this->num = 0;			// current fruit ID
	this->maxNum = 0;			// maximum number of fruit (as per sprite sheet)
	this->value = 100;			// value of fruit
	this->newSpawnSet = true;
}

Bonus::~Bonus() {

}

void Bonus::Draw(int frameCount) {
	if (this->isShown) {
		Entity::Draw(frameCount);
	}
}

int Bonus::GetValue() {
	return (this->num + 1) * this->value;
}

void Bonus::NextBonus() {
	if (this->num <= this->maxNum) {
		this->num++;
	} else {
		this->num = 0;
	}
}

void Bonus::SetPosition(Vector2i pos) {
	this->position->X = pos.X;
	this->position->Y = pos.Y;
	this->newSpawnSet = true;
}

void Bonus::Update(int elapsedTime) {
	// Animate Bonus
	Entity::Update(elapsedTime);
	
	// Select current fruit
	this->sourceRect->Y = this->sourceRect->Height * this->num;

	// Auto spawn timer
	if (this->spawnTimer >= this->spawnMaxTime) {
		this->spawnTimer = 0;
	}
	else {
		this->spawnTimer++;
	}

	if (this->spawnTimer >= this->spawnDelay) {
		if (!this->isShown) {
			this->isShown = true;
		} else {
			this->isShown = false;
			this->newSpawnSet = false;
		}

		this->spawnTimer = 0;
	}
}