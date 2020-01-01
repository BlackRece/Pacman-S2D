#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Motion.h"
#include "Vector2i.h"

class Player : public Entity, virtual public Motion
{
private:
	Direction facing;
	int lives;

	int boostDuration;
	int boostDurationMax;
	float boostMultiply;
	float baseSpeed;
	
public:
	bool isBoosting;
	Movement nextMove;
	Movement currMove;

	Player();
	~Player();

	Vector2* GetPosFloat();
	Vector2i* GetPosInt();
	void MoveBy(Vector2 velocity);
	void SetPosInt(Vector2i newPos);
	void SetPosFloat(Vector2 newPos);
	void StartBoosting();
	void StopBoosting();

	void Update(int elapsedTime = 0);
	void UpdateBoost();
	void Player::UpdateFacing();

};

#endif