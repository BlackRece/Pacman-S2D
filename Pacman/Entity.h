#ifndef ENTITY_H
#define ENTITY_H

#include "S2D/S2D.h"
#include "Vector2i.h"

using namespace S2D;

class Entity 
{
private:
	//animation
	int currentFrame;
	int maxFrame;
	int delayFrame;
	int delayCounter;

	//motion
	float speed;

public:
	//texture
	Texture2D* texture;
	Rect* sourceRect;
	Rect* position;

	float scale;
	int offset;

	Entity();
	~Entity();

	void virtual Draw(int frameCount = 0);
	
	int GetFrame();

	int Entity::GetMaxFrames();
	
	Vector2* GetPosFloat();
	
	Vector2i* GetPosInt();
	
	float GetSpeed();
	
	void MoveBy(Vector2 velocity);
	
	void NextFrame();
	
	void SetPosInt(Vector2i newPos);

	void SetPosFloat(Vector2 newPos);

	/// <summary> Auto calculates no of frames based on size of sprite/frame. </summary>
	void SetFrames(int tileSize);

	void SetFrameDelay(int delayBy = 0);

	void SetMaxFrames(int numOfFrames = 0);

	void SetOffset(int newOffset);

	void SetSpeed(float newSpeed = 0);

	void SetTexture(Texture2D * source);

	void virtual Update(int elapsedTime);

	void UpdateFrame();
};

#endif