#pragma once
#define TILE_SIZE 32
#define MAX_ELEMENTS 644
#define WWIDTH 1024
#define WHEIGHT 768
#define NUM_OF_MUNCHIES 210
#define MAP_ROWS 23
#define MAP_COLS 27

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
	Rect* position;
	Rect* sourceRect;
};

// any element that pacman has to negotiate ie walls and doors
struct Obstacle {
	Entity self;

	bool canGhostsPass;
	bool canGhostsSpawn;

	bool canPlayerPass;
	bool canPlayerSpawn;

	bool canBonusSpawn;
};

// any element that can harm pacman ie traps and ghosts
struct Hazard {
	Entity self;
};

//data for munchies
struct Munchie {
	Entity self;

	float offset;
	bool scale;
	bool isPowerPellet;

	bool isEaten;
	int value;
};

//data for collectables
struct Bonus {
	Entity self;

	bool spawn;
	bool isShown;
	int value;
};

//data for ghosts
struct Enemy {
	Entity self;

	bool isChasing;
	bool isAlive;
};

struct Menu {
	Texture2D* _backGround;
	Rect* _rect;
	Rect* _textRect;
	Vector2* _stringPosition;
	string text;
	bool _menu;	// better name to describe action
	bool _keyDown;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	
	int _pacmanFrame;
	int _pacmanCurrentFrameTime;
	int _frameCount;
	int _time = 0;

	char map[MAP_ROWS][MAP_COLS];

	Menu* _pause;
	Menu* _start;
	bool _mainMenu;
	
	Player* _pacman;

	//data to represent munchies
	Entity* _munchies[NUM_OF_MUNCHIES];
	Texture2D* _munchieTexture;
	//int _munchieCounter;

	Bonus* _cherry;
	Bonus* _powerUp;

	//data to represent border
	const static int NUM_OF_WALLS = MAX_ELEMENTS - NUM_OF_MUNCHIES;
	Obstacle* _walls[NUM_OF_WALLS];
	Texture2D* _wallTexture;
	//int _wallCounter;
	int _wallScale;


	// Position for Strings
	Vector2* _stringPosition;
	Vector2* _stringScore;
	Vector2* _stringDirection;

	int score = 0;

	void Input(int elapsedTime, Input::KeyboardState* state);

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);

	void CheckViewPortCollision();

	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);

	bool collisionCheck(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);
	
	void DeleteEntity(Entity* obj);

	void DeleteMenu(Menu obj);

	void DeleteObstacle(Obstacle* obj);

	void DeletePlayer(Player* obj);
	
	void DeleteSelf(Entity* obj);

	void DeleteMunchie(Munchie* obj);
	
	void UpdatePacman(int elapsedTime);

	void UpdateMunchie(int elapsedTime);

	void UpdateCherry(int elapsedTime);



public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();


	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called once to load text file. </summary>
	void LoadMap();
	
	/// <summary> Called once to initialise map elements from loaded map array. </summary>
	void InitialiseMap();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

};