#ifndef MUNCHIE_H
#define MUNCHIE_H

#include "Entity.h"

class Munchie : public Entity
{
private:
	//void Update();

public:
	bool isPowerPellet;
	bool isEaten;
	int value;

	Munchie(int munchieSize = 0);
	~Munchie();

	void virtual Draw(int frameCount = 0);

};

#endif