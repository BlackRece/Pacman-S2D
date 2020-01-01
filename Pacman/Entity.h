#ifndef ENTITY_H
#define ENTITY_H

#include "S2D/S2D.h"

using namespace S2D;

class Entity 
{
private:
	//texture
	Texture2D* texture;
	float scale;

	//animation
	int currentFrame;
	int maxFrame;
	int delayFrame;
	int delayCounter;

	//motion
	float speed;

public:
	Rect* position;
	Rect* sourceRect;
	int offset;

	Entity();
	~Entity();

	void virtual Draw(int frameCount = 0);
	int GetFrame();
	float GetSpeed();
	
	void NextFrame();

	/// <summary> Auto calculates no of frames based on size of sprite/frame. </summary>
	void SetFrames(int tileSize);

	void SetFrameDelay(int delayBy = 0);

	void SetMaxFrames(int numOfFrames = 0);

	void Entity::SetOffset(int newOffset);

	void SetSpeed(float newSpeed = 0);

	void SetTexture(Texture2D * source);

	void virtual Update(int elapsedTime);
};

#endif