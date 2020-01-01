#ifndef MOTION_H
#define MOTION_H

enum class Direction : unsigned int {
	dRIGHT = 0,
	dDOWN,
	dLEFT,
	dUP,
	dNULL
};

enum class Movement : unsigned int {
	mLEFT = 0,
	mRIGHT,
	mUP,
	mDOWN,
	mSTOP
};

class Motion
{
private:

public:
	Direction IsFacing(Movement movement);

	Movement RandomMotion();
};

#endif