#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Motion.h"

enum class GhostType : unsigned int {
	Blinky = 0,
	Inky,
	Clyde,
	Pinky
};

class Enemy : public Entity, public virtual Motion
{
private:
	Direction facing;

	int fearCounter;
	int fearDuration;

	/*
	int boostDuration;
	int boostDurationMax;
	float boostMultiply;
	float baseSpeed;
	*/

public:
	GhostType persona;
	bool isBoosting;
	bool isAlive;
	bool isChasing;
	int value;
	Movement nextMove;
	Movement currMove;

	Enemy();
	~Enemy();

	void StartFlapping();

	void Update(int elapsedTime = 0);

	void Enemy::UpdateFacing();

	/*
	void StartBoosting();
	
	void StopBoosting();

	void Update(int elapsedTime = 0);
	
	void UpdateBoost();
	*/

};

#endif