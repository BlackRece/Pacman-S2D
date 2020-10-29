#include "Player.h"

Player::Player() {
	lives = 3;
	deathThrows = false;
	deathCounter = 0;
	deathDelay = 20;
	deathFrame = 0;
	currMove = Movement::mSTOP;
	nextMove = Movement::mSTOP;
	facing = Direction::dLEFT;
	boostDuration = 10;
	boostDurationMax = 10;
	boostMultiply = 2;
	isBoosting = false;
	SetSpeed(0);
	baseSpeed = 0;
}

Player::~Player() {

}

bool Player::DeathSequence() {
	if (deathThrows) {
		if (deathCounter < deathDelay) {
			deathCounter++;
		} else {
			// reset counter
			deathCounter = 0;

			// select death frame animation
			sourceRect->X = sourceRect->Width * 2;
			sourceRect->Y = sourceRect->Height * deathFrame;

			// increment death frame
			deathFrame++;

			if (deathFrame > 4) {
				// reset deathFrame counter
				deathFrame = 0;

				// finish death  sequence
				deathThrows = false;
			}
		}
	}

	return deathThrows;
}

void Player::StartBoosting() {
	if ((!isBoosting) && 
		(boostDuration == 0))
	{
		isBoosting = true;
		baseSpeed = GetSpeed();
		SetSpeed(baseSpeed * boostMultiply);
	} 
		
}

void Player::StopBoosting() {
	if ((isBoosting) &&
		(boostDuration == 0))
	{
		isBoosting = false;
		SetSpeed(baseSpeed);
		baseSpeed = 0;
	}
}

void Player::Update(int elapsedTime) {
	// Animation
	Entity::Update(elapsedTime);

	if (!deathThrows) {
		// Update Boosting State
		UpdateBoost();

		// Update Movement
		UpdateFacing();
	} else {

	}
}

void Player::UpdateBoost() {
	if (isBoosting) {
		if (boostDuration < boostDurationMax) {
			boostDuration++;
		} else {
			StopBoosting();
			isBoosting = false;
		}
	} else {
		if (boostDuration > 0) {
			boostDuration--;
		}
	}
}

void Player::UpdateFacing() {
	// apply facing direction to sprite
	if (facing != IsFacing(currMove) &&
		IsFacing(currMove) != Direction::dNULL) {
		facing = IsFacing(currMove);
	}

	// uses the order of Direction enum to determine which frame to use
	sourceRect->Y = sourceRect->Height * (int)facing;
}