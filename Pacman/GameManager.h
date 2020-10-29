#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#define TILE_SIZE 32
#define MAX_FRAMES 60
#define MAX_ELEMENTS 644
#define WWIDTH 1184
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
#include "Enemy.h"
#include "Munchie.h"
#include "Bonus.h"
#include "Obstacle.h"
#include "LevelManager.h"
#include "BillBoard.h"
#include "HUD.h"

using namespace S2D;

enum class GameState : unsigned int {
	gsStarting = 0,	// game is starting - initialation and loading
	gsPlaying,		// game is playing - all entites are moving
	gsRunning,		// * game is running - pacman not moving?
	gsPausing,		// game is paused - no entities are moving
	gsStopping,		// * game is ending - destroy all objects and end
	gsScoring,		// game is showing high scores - no entities are drawn
	gsDeath			// pause game while playing death animation
};

class GameManager : public Game
{
private:
	//debug
	bool debugFlag;

	/* VARIABLES */

	int _frameCount;
	int _time;
	GameState _currentState;

	/* Players */
	//PacMan
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;

	Player* _pacman;

	//Ghosts
	Enemy* _ghosts;
	Texture2D* _ghostTexture;
	
	/* World */
	LevelManager* _level;

	/* Obstacles */
	//Walls
	Obstacle* _walls;
	Texture2D* _wallTexture;

	/* Collectables */

	//Munchies
	Munchie* _munchies;
	Texture2D* _munchieTexture;

	//Fruits
	Bonus* _fruit;

	/* Music & Sound Effects */
	bool introPlayed;
	SoundEffect* _pop;
	SoundEffect* _intro;
	SoundEffect* _death;
	SoundEffect* _eatFruit;
	SoundEffect* _eatGhost;
	SoundEffect* _nextLevel;

	/* Menus & HUD */
	BillBoard* _status;
	HUD* _hud;
	
	int score;

	/* METHODS */

	Vector2 ApplyMovementF(Movement direction, float velocity);

	Vector2i ApplyMovementI(Movement direction, int velocity);

	void CheckCollisions();

	bool CheckMapCollision(Vector2 pos, Movement currentMove);

	Movement CheckMotion(Vector2i pos, Movement nextmove, GhostType mood);

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey);

	void CheckViewportCollision(Rect* pos);

	void DefineMap();

	Movement ForwardMotion(Vector2i pos, Movement move);

	/// <summary> Called once to initialise map elements from loaded map array. </summary>
	void InitialiseGhosts();

	void InitialiseMunchies();

	void InitialiseWalls();

	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	bool IsPassable(Vector2i pos, Movement move, bool isPlayer);

	bool HasTargetHitObject(Rect* git, Rect* obj, float tolerance = 0, char mode = 'c');

	Movement NotSoRandomMotion(Vector2i pos);

	void ResetGame(bool nextLevel = false);

	void ResetGhosts();

	void ResetPlayer();

	void ScareGhosts();

	void UpdateGhosts(int elapsedTime = 0);

	void UpdateMunchies();

	void UpdatePacman(int elapsedTime = 0);

public:
	/// <summary> Constructs the Pacman class. </summary>
	GameManager(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~GameManager();

	Movement ChaseMotion(Vector2i git, Vector2i obj);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	float DistanceToTarget(Vector2i obj, Vector2i git);

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

};

#endif