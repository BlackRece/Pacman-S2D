#include "Player.h"

Player::Player() {
	this->lives = 3;
	this->currMove = Movement::mSTOP;
	this->nextMove = Movement::mSTOP;
	this->facing = Direction::dLEFT;
	this->boostDuration = 10;
	this->boostDurationMax = 10;
	this->boostMultiply = 2;
	this->isBoosting = false;
	this->SetSpeed(0);
	this->baseSpeed = 0;
}

Player::~Player() {

}

Vector2* Player::GetPosFloat() {
	return new Vector2(this->position->X,this->position->Y);
}

Vector2i* Player::GetPosInt() {
	return new Vector2i(floor(this->position->X), floor(this->position->Y));
}

void Player::MoveBy(Vector2 velocity) {
	this->position->X += velocity.X;
	this->position->Y += velocity.Y;
}

void Player::SetPosInt(Vector2i newPos) {
	this->position->X = newPos.X;
	this->position->Y = newPos.Y;
}

void Player::SetPosFloat(Vector2 newPos) {
	this->position->X = newPos.X;
	this->position->Y = newPos.Y;
}

void Player::StartBoosting() {
	if ((!this->isBoosting) && 
		(this->boostDuration == 0))
	{
		this->isBoosting = true;
		this->baseSpeed = this->GetSpeed();
		this->SetSpeed(this->baseSpeed * this->boostMultiply);
	} 
		
}

void Player::StopBoosting() {
	if ((this->isBoosting) &&
		(this->boostDuration == 0))
	{
		this->isBoosting = false;
		this->SetSpeed(this->baseSpeed);
		this->baseSpeed = 0;
	}
}

void Player::Update(int elapsedTime) {
	// Animation
	Entity::Update(elapsedTime);

	//TODO: Update facing direction

	// Update Boosting State
	this->UpdateBoost();

	// Update Movement
	this->UpdateFacing();

}

void Player::UpdateBoost() {
	if (this->isBoosting) {
		if (this->boostDuration < this->boostDurationMax) {
			this->boostDuration++;
		} else {
			this->StopBoosting();
			this->isBoosting = false;
		}
	} else {
		if (this->boostDuration > 0) {
			this->boostDuration--;
		}
	}
}

void Player::UpdateFacing() {
	// apply facing direction to sprite
	if (this->facing != IsFacing(this->currMove) &&
		IsFacing(this->currMove) != Direction::dNULL) {
		this->facing = IsFacing(this->currMove);
	}

	// uses the order of Direction enum to determine which frame to use
	this->sourceRect->Y = this->sourceRect->Height * (int)this->facing;
}