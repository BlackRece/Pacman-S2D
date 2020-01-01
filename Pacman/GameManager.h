#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#define TILE_SIZE 32
#define MAX_FRAMES 60
#define MAX_ELEMENTS 644
#define WWIDTH 1028
#define WHEIGHT 768
#define MAP_ROWS 23
#define MAP_COLS 27
#define NUM_OF_GHOSTS 8
#define NUM_OF_FRUITS 8
#define NUM_OF_MUNCHIES 210
#define MUNCHIE_SIZE 16

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

//S2D engine, in all of its glory...
#include "S2D/S2D.h"

//custom game features
#include "Player.h"
#include "Munchie.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "LevelManager.h"

using namespace S2D;

enum class GameState : unsigned int {
	gsStarting = 0,	// game is starting - initialation and loading
	gsPlaying,		// game is playing - all entites are moving
	gsRunning,		// * game is running - pacman not moving?
	gsPausing,		// game is paused - no entities are moving
	gsStopping,		// * game is ending - destroy all objects and end
	gsScoring		// game is showing high scores - no entities are drawn
};

class GameManager : public Game
{
private:
	int _frameCount;
	int _time;
	GameState _currentState;

	//data to represent player (aka PacMan)
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;

	Player* _pacman;
	Texture2D* _pacmanTexture;
	//int _pacmanFrame;
	//int _pacmanCurrentFrameTime;

	//TODO: create a map class
	//char map[MAP_ROWS][MAP_COLS];
	LevelManager* _level;

	//data to represent walls
	Obstacle* _walls;
	Texture2D* _wallTexture;

	/* Collectables */

	//data to represent munchies
	Munchie* _munchies;
	Texture2D* _munchieTexture;
	int _munchieCount;		//dont need

	//data to represent fruits
	Bonus* _fruit;
	Texture2D* _fruitTexture;

	//TODO: create SoundManager class!
	SoundEffect* _pop;
	SoundEffect* _intro;
	SoundEffect* _death;
	SoundEffect* _eatFruit;
	SoundEffect* _eatGhost;
	SoundEffect* _nextLevel;

	// Position for Strings
	//TODO: create HUD class!
	/*
	Vector2* _stringPosition;
	Vector2* _stringScore;
	Vector2* _stringDirection;
	*/

	int score;

	Vector2 ApplyMovementF(Movement direction, float velocity);

	Vector2i ApplyMovementI(Movement direction, int velocity);

	void CheckCollisions();

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey);

	void CheckViewportCollision(Rect* pos);

	void DefineMap();

	bool HasTargetHitObject(Rect* git, Rect* obj, float tolerance = 0, char mode = 'c');

	/// <summary> Called once to initialise map elements from loaded map array. </summary>
	void InitialiseGhosts();

	void InitialiseMunchies(int numOfMunchies);

	void InitialiseWalls(int numOfWalls);

	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	bool IsPassable(Vector2i pos, Movement move, bool isPlayer);

	/// <summary> Called once to load text file. </summary>
	void LoadMap();

	void UpdatePacman(int elapsedTime);

public:
	/// <summary> Constructs the Pacman class. </summary>
	GameManager(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~GameManager();


	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

};

#endif