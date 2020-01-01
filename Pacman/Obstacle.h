#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Entity.h"
class Obstacle : public Entity
{
private:

public:
	Obstacle();
	~Obstacle();

	bool canGhostsPass;
	bool canGhostsSpawn;

	bool canPlayerPass;
	bool canPlayerSpawn;

	bool canBonusSpawn;
};

#endif