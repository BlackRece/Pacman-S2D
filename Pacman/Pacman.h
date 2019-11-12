#pragma once
#define TILE_SIZE 32
#define MAX_ELEMENTS 644

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

enum Movement {
	mLEFT = 0,
	mRIGHT,
	mUP,
	mDOWN,
	mSTOP
};

enum Direction {
	dRIGHT = 0,
	dDOWN,
	dLEFT,
	dUP
};

// Data to represent a player
struct Player {
	Rect* sourceRect;
	Texture2D* texture;
	int frame; 
	int currentFrametime;
	Direction facing;

	Vector2* position;
	Vector2* direction;
	Movement nextMove;
	Movement currMove;
	Movement lastMove;
	float speed;

	int score;
};

// every element that requires basic representation
struct Entity {
	Texture2D* texture;
	Rect* 
};

// any element that pacman has to negotiate ie walls and doors
struct Obstacle {

};

// any element that can harm pacman ie traps and ghosts
struct Hazard {

};
struct Collectable {
	//data for collectables
	Vector2* _Position;
	Rect* _SrcRect;
	Texture2D* _Texture;
	int _rand;
	bool spawn;
};

struct Menu {
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _menu;
	bool _keyDown;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	void Input(int elapsedTime, Input::KeyboardState* state);

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);

	void CheckViewPortCollision();

	void UpdatePacman(int elapsedTime);

	void UpdateMunchie(int elapsedTime);

	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);

	void UpdateCherry(int elapsedTime);

	bool collisionCheck(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);

	int _frameCount;
	int _time = 0;

	Player* _pacman;
	Collectable* _munchie;
	Collectable* _cherry;
	Collectable* _powerUp;
	Menu* _pause;
	Menu* _start;

	//data to represent border
	Vector2* _borderPosition;
	Rect* _borderSrcRect;
	Texture2D* _borderTexture;

	// Position for String
	Vector2* _stringPosition;
	Vector2* _scorePosition;
	int score = 0;
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};