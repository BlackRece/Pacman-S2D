#include "Obstacle.h"

Obstacle::Obstacle()
{
	canGhostsPass = false;
	canGhostsSpawn = false;

	canPlayerPass = false;
	canPlayerSpawn = false;

	canBonusSpawn = false;

	mapCoords = *Vector2i::Zero;
}

Obstacle::~Obstacle()
{
}
