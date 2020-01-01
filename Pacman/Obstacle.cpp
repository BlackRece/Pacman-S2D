#include "Obstacle.h"

Obstacle::Obstacle()
{
	this->canGhostsPass = false;
	this->canGhostsSpawn = false;

	this->canPlayerPass = false;
	this->canPlayerSpawn = false;

	this->canBonusSpawn = false;
}

Obstacle::~Obstacle()
{
}
