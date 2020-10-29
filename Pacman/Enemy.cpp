#include "Enemy.h"

Enemy::Enemy() {
	currMove = Movement::mSTOP;
	nextMove = Movement::mSTOP;
	facing = Direction::dLEFT;
	isBoosting = false;
	isAlive = false;
	isChasing = true;
	value = 100;
	persona = GhostType::Blinky;
	SetSpeed(0.11f);

	fearCounter = 0;
	fearDuration = 400;

	/*
	boostDuration = 10;
	boostDurationMax = 10;
	boostMultiply = 2;
	SetSpeed(0);
	baseSpeed = 0;
	*/
}

Enemy::~Enemy() {

}

void Enemy::StartFlapping() {
	isChasing = false;
	SetSpeed(0.06f);
}

void Enemy::Update(int elapsedTime) {
	UpdateFrame();
	int thisFrame = GetFrame();


	if (isChasing) {
		//select correct sprites set/colum
		sourceRect->X = 
			sourceRect->Width * (((int)persona * GetMaxFrames()) + GetFrame());

		UpdateFacing();
	} else {
		//depending on fear duration
		if (fearCounter > fearDuration) {
			fearCounter = 0;
			isChasing = true;
			SetSpeed(0.11f);
		} else {
			if (fearCounter > fearDuration - (fearDuration / 4)) {
				//flashing
				//select scared sprite set colum
				sourceRect->X = sourceRect->Width * 
					((5 * GetMaxFrames()) + GetFrame());
			} else {
				//select scared sprite set colum
				sourceRect->X = sourceRect->Width * 
					((4 * GetMaxFrames()) + GetFrame());

			}

			fearCounter++;
		}

	}

}

void Enemy::UpdateFacing() {
	// apply facing direction to sprite
	if (facing != IsFacing(currMove) &&
		IsFacing(currMove) != Direction::dNULL) {
		facing = IsFacing(currMove);
	}

	// uses the order of Direction enum to determine which frame to use
	sourceRect->Y = sourceRect->Height * (int)facing;
}