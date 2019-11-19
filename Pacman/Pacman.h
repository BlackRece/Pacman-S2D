#pragma once
#define TILE_SIZE 32
#define MAX_ELEMENTS 644
#define WWIDTH 1024
#define WHEIGHT 768
#define NUM_OF_MUNCHIES 210
#define MAP_ROWS 23
#define MAP_COLS 27
#define NUM_OF_GHOSTS 4

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

struct Vector2i {
	int X = 0;
	int Y = 0;
};

enum class Movement : unsigned int {
	mLEFT = 0,
	mRIGHT,
	mUP,
	mDOWN,
	mSTOP
};

enum class Direction : unsigned int {
	dRIGHT = 0,
	dDOWN,
	dLEFT,
	dUP,
	dNULL
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
	bool isBoosting;
	int boostDuration;
	int boostDurationMax;
	float boostMultiply;

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
	float scale;
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

	int currFrame;
	int maxFrame;
	bool isChasing;
	bool isAlive;
	bool isSafe;

	Direction facing;
	Movement motion;
	float speed;
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
	Munchie* _munchies[NUM_OF_MUNCHIES] = { nullptr };
	Texture2D* _munchieTexture;

	Bonus* _cherry;
	Bonus* _powerUp;

	//data to represent border
	const static int NUM_OF_WALLS = MAX_ELEMENTS - NUM_OF_MUNCHIES;
	Obstacle* _walls[NUM_OF_WALLS] = { nullptr };
	Texture2D* _wallTexture;
	int _wallScale;

	//data to represent ghosts
	Enemy* _ghosts[NUM_OF_GHOSTS];
	Texture2D* _ghostTexture;
	int _ghostFearTimer;
	int _ghostFearTimerMax;

	// Position for Strings
	Vector2* _stringPosition;
	Vector2* _stringScore;
	Vector2* _stringDirection;

	int score = 0;

	Vector2 ApplyMovement(Movement direction, float velocity);


	void CheckPacmanCollision();

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey);

	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);

	void CheckViewportCollision();

	void DeleteEntity(Entity* obj);

	void DeleteMenu(Menu* obj);

	void DeleteObstacle(Obstacle* obj);

	void DeletePlayer(Player* obj);
	
	void DeleteSelf(Entity* obj);

	void DeleteMunchie(Munchie* obj);
	
	Movement GetMapMovement(Vector2i tile);
	
	std::string GetMovementString(Movement movement);

	bool HasHitWall(Rect* target, bool isPlayer, float targetTolerance = 0, float wallTolerance = 0);

	bool HasTargetHitObject(Rect* git, Rect* obj, float tolerance = 0, char mode = 'c');

	void Input(int elapsedTime, Input::KeyboardState* state);

	Direction IsFacing(Movement movement);

	bool IsSpaceTile(Vector2i origin, Movement moveTo);

	Movement RandomMotion();

	void ScareGhosts();

	void UpdateCherry(int elapsedTime);

	void UpdateGhost(int elapsedTime);

	void UpdateMunchie(int elapsedTime);

	void UpdatePacman(int elapsedTime);

	bool WillHitWall(Rect* target, Movement targetMove, float targetSpd);


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
	void InitialiseGhosts();

	void InitialiseMunchies();

	void InitialiseWalls();

	void DefineMap();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

};