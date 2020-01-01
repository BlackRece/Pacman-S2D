#include "Motion.h"
#include <stdlib.h>

Direction Motion::IsFacing(Movement movement) {
	switch (movement) {
	case Movement::mDOWN:
		return Direction::dDOWN;
		break;
	case Movement::mRIGHT:
		return Direction::dRIGHT;
		break;
	case Movement::mLEFT:
		return Direction::dLEFT;
		break;
	case Movement::mUP:
		return Direction::dUP;
		break;
	default:
		return Direction::dNULL;
		break;
	}
}

Movement Motion::RandomMotion() {
	switch (rand() % 4) {
	case 0:
		return Movement::mDOWN;
		break;
	case 1:
		return Movement::mLEFT;
		break;
	case 2:
		return Movement::mRIGHT;
		break;
	case 3:
		return Movement::mUP;
		break;
	}
}