#ifndef BONUS_H
#define BONUS_H

#include "Entity.h"
#include "Vector2i.h"

class Bonus : public Entity
{
private:

public:
	int spawnMaxTime;	// maximum duration before timer resets
	int spawnTimer;		// current counter 
	int spawnDelay;		// ...
	bool isShown;
	bool newSpawnSet;
	int num;			// current fruit ID
	int maxNum;			// maximum number of fruit (as per sprite sheet)
	int value;			// value of fruit

	Bonus();
	~Bonus();

	void Bonus::Draw(int frameCount);

	int GetValue();

	void NextBonus();

	void SetPosition(Vector2i pos);
	
	void Update(int elapsedTime = 0);
};

#endif // !BONUS_H