#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Entity.h"
#include "Vector2i.h"
#include <string>

class BillBoard : public Entity
{
private:
	string caption;
	bool isPaused;
public:
	Rect* textRect;
	Vector2i textPos;
	bool pauseKeyDown;
	bool startKeyDown;

	Texture2D *title;

	BillBoard();
	~BillBoard();

	void BillBoard::ConfigState(bool justPaused = true);

	void Draw(int frameCount = 0);

	void DrawTitle();

	void BillBoard::SetString(string text);
};

#endif