#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Motion.h"

class Player : public Entity, virtual public Motion
{
private:
	Direction facing;
	int deathCounter;
	int deathDelay;
	int deathFrame;

	int boostDuration;
	int boostDurationMax;
	float boostMultiply;
	float baseSpeed;
	
public:
	bool isBoosting;

	Movement nextMove;
	Movement currMove;

	bool deathThrows;
	int lives;

	Player();
	~Player();

	bool DeathSequence();

	void StartBoosting();

	void StopBoosting();

	void Update(int elapsedTime = 0);
	
	void UpdateBoost();
	
	void UpdateFacing();

};

#endif