#include "GameManager.h"
#include <time.h>
#include <iostream>

using namespace std;

GameManager::GameManager(int argc, char* argv[]) :
	Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(30)
{
	//debug
	debugFlag = false;

	_frameCount = 0;
	_time = 0;
	_currentState = GameState::gsStarting;

	// initialise level
	_level = new LevelManager();

	/* Players */

	// initialise pacman
	_pacman = new Player();
	_pacman->SetMaxFrames(2);
	_pacman->SetSpeed(_cPacmanSpeed);
	_pacman->sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
	_pacman->SetFrameDelay(_cPacmanFrameTime);
	_pacman->SetOffset(TILE_SIZE / 2);

	// initialise ghosts
	_ghosts = new Enemy[NUM_OF_GHOSTS];
	_ghostTexture = new Texture2D();

	/* Collectables */

	// initialise munchies
	_munchies = new Munchie[_level->CountTiles(TileType::tMUNCHIE)];
	_munchieTexture = new Texture2D();

	// initialise fruits
	_fruit = new Bonus();
	_fruit->maxNum = 8;
	_fruit->sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
	_fruit->SetOffset(TILE_SIZE / 2);
		
	/* Obstacles */
	// initialise walls
	_walls = new Obstacle[_level->CountTiles(TileType::tWALL)];
	_wallTexture = new Texture2D();

	// music & soundeffects
	_pop = new SoundEffect();
	_intro = new SoundEffect();
	_death = new SoundEffect();
	_eatFruit = new SoundEffect();
	_eatGhost = new SoundEffect();
	_nextLevel = new SoundEffect();
	introPlayed = false;

	/* HUD */
	score = 0;
	_status = new BillBoard();
	_hud = new HUD();
	
	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, WWIDTH, WHEIGHT, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

GameManager::~GameManager() {
	/* Textures */
	delete _munchieTexture;
	delete _wallTexture;
	delete _ghostTexture;

	/* Audio */
	delete _pop;
	delete _intro;
	delete _death;
	delete _eatFruit;
	delete _eatGhost;
	delete _nextLevel;
}

Vector2 GameManager::ApplyMovementF(Movement direction, float velocity){
	Vector2 tmpDir = Vector2();

	switch (direction) {
	case Movement::mDOWN:
		//Moves Down Y axis
		tmpDir.Y = velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mUP:
		//Moves up Y axis
		tmpDir.Y = -velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mLEFT:
		//Moves Left X axis
		tmpDir.X = -velocity;

		//Resets X axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mRIGHT:
		//Moves Right X axis
		tmpDir.X = velocity;

		//Resets Y axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mSTOP:
		//target stops moving
	default:
		//reset both axis
		tmpDir.Y = 0.0f;
		tmpDir.X = 0.0f;
		break;
	}

	return tmpDir;
}

Vector2i GameManager::ApplyMovementI(Movement direction, int velocity){
	Vector2i tmpDir = Vector2i();

	switch (direction) {
	case Movement::mDOWN:
		//Moves Down Y axis
		tmpDir.Y = velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mUP:
		//Moves up Y axis
		tmpDir.Y = -velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mLEFT:
		//Moves Left X axis
		tmpDir.X = -velocity;

		//Resets X axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mRIGHT:
		//Moves Right X axis
		tmpDir.X = velocity;

		//Resets Y axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mSTOP:
		//target stops moving
	default:
		//reset both axis
		tmpDir.Y = 0.0f;
		tmpDir.X = 0.0f;
		break;
	}

	return tmpDir;
}

void GameManager::CheckCollisions() {
	/* Collectables */
	// bonuses (fruits and power-ups)
	if (_fruit->isShown) {
		if (HasTargetHitObject(_pacman->position, _fruit->position)) {
			//hide fruit
			_fruit->isShown = false;

			//update score
			score += _fruit->GetValue();

			//play soundfx
			Audio::Play(_eatFruit);

			//ready next bonus
			_fruit->NextBonus();

			//reset bonus timer/delay
			_fruit->spawnTimer = 0;
		}
	} else {
		//get new position for bonus(fruit)
		//NOTE: will only spawn where munchies are, excluding power pellets
		_fruit->SetPosition(_level->GetRandomTilePos(TileType::tMUNCHIE) * TILE_SIZE);
	}

	// munchies and power pellets
	for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
		if (!_munchies[i].isEaten) {
			//test munchie position
			if (HasTargetHitObject(_pacman->position, _munchies[i].position)) {
				//set flag
				_munchies[i].isEaten = true;

				//update score
				score += _munchies[i].value;

				if (_munchies[i].isPowerPellet) {
					//play soundfx
					Audio::Play(_eatFruit);

					//scare ghosts!
					ScareGhosts();
				} else {
					//play soundfx
					Audio::Play(_pop);
				}

			}
		}
	}

	// ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		if (HasTargetHitObject(_pacman->position, _ghosts[i].position, 0, 'b')) {
			if (_ghosts[i].isChasing) {
				//set game state
				_currentState = GameState::gsDeath;

				//play sound
				Audio::Play(_death);
				
				//kill player
				_pacman->lives--;
				_pacman->deathThrows = true;

			} else {
				//kill ghost
				_ghosts[i].SetPosInt(_level->GetRandomTilePos(TileType::tGHOST) * TILE_SIZE);

				//update score
				score += _ghosts[i].value;

				//play soundfx
				Audio::Play(_eatGhost);
			}
		}
	}
}

bool GameManager::CheckMapCollision(Vector2 pos, Movement currentMove) {
	pos /= TILE_SIZE;
	return _level->IsMapTile(pos.Y, pos.X, TileType::tWALL);
}

Movement GameManager::CheckMotion(Vector2i pos, Movement nextmove, GhostType mood) {
	Movement result = Movement::mSTOP;

	// placeholder for the backwards movement
	//NOTE: doesn't account for corner scenarios
	Movement backwards = Movement();			
	Movement moves[4] = {						// array of movement choices available
		Movement::mSTOP, Movement::mSTOP,
		Movement::mSTOP, Movement::mSTOP
	};

	int checkMoves = 0;							// counter to check number of moves available
	int nearest = MAX_ELEMENTS * TILE_SIZE;					// arbitrary large number
	int distances;							// array of distances between ghost and target
	Vector2i targetPos = Vector2i();
	
	//work out the backwards movement
	switch (nextmove) {
	case Movement::mLEFT:
		backwards = Movement::mRIGHT;
		break;
	case Movement::mRIGHT:
		backwards = Movement::mLEFT;
		break;
	case Movement::mUP:
		backwards = Movement::mDOWN;
		break;
	case Movement::mDOWN:
		backwards = Movement::mUP;
		break;
	default:
		break;
	}

	switch (mood) {
	case GhostType::Blinky:
		//go to pacman's current tile position
		targetPos = *_pacman->GetPosInt();
		break;
	case GhostType::Inky:
		//go to position 3 tiles in front of pacamn
		targetPos = ApplyMovementI(_pacman->nextMove, _pacman->GetSpeed()) * 3;
		targetPos += *_pacman->GetPosInt();
		break;
	case GhostType::Pinky:
		//go to position 3 tiles behind of pacamn
		targetPos = ApplyMovementI(_pacman->nextMove, _pacman->GetSpeed() * -1) * 3;
		targetPos += *_pacman->GetPosInt();
		break;
	case GhostType::Clyde:
		//targetPos = ApplyMovementI(nextmove, TILE_SIZE);
		//go in a random direction
		targetPos = pos;
		break;
	default:	//choose random direction
		break;
	}

	// fill array with available movements
	moves[0] = _level->IsMapTile(pos.Y, pos.X - 1, TileType::tWALL) ? Movement::mSTOP : Movement::mLEFT;
	moves[1] = _level->IsMapTile(pos.Y, pos.X + 1, TileType::tWALL) ? Movement::mSTOP : Movement::mRIGHT;
	moves[2] = _level->IsMapTile(pos.Y - 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mUP;
	moves[3] = _level->IsMapTile(pos.Y + 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mDOWN;

	// count available movements
	for (int i = 0; i < 4; i++) {
		if (moves[i] != Movement::mSTOP) {
			checkMoves++;

			//store a valid move
			result = moves[i];
		}
	}

	// check how many moves are available
	switch (checkMoves) {
		/*
		- zero moves means the ghosst has got stuck or some other error
		- one move is the direction this ghost came from.
		- two moves is either:
			a) the direction the ghost was already heading
			- or -
			b) the only other direction available WITHOUT going backwards
		- three or more moves means the ghost has arrived at an intersection
		and has a choice of routes to take
		*/
	case 0:
		//DEBUG
		cout << "CheckMotion: No moves found!" << endl;
		break;
	case 1:
		//DEBUG
		cout << "CheckMotion: Dead end?! One move found!" << endl;

		//case a = if current move was valid, then we already have it stored
		//return the single valid move
		return result;
		break;
	case 2:
		//case b = find the remaining valid move

		//loop through stored moves
		for (int i = 0; i < 4; i++) {
			//store current move, if valid
			if (moves[i] == nextmove) {
				result = moves[i];
				break;
			}
		}
		break;
	default:
		//reset result since there are more than ONE valid moves
		result = Movement::mSTOP;

		if (targetPos == pos) {
			//get random direction
			while (result == Movement::mSTOP) {
				result = moves[rand() % 4];

				//excluding backwards
				if (result == backwards) {
					result == Movement::mSTOP;
				}
			}
		} else {
			//get direction based on targetPos
			for (int i = 0; i < 4; i++) {
				//calc distance between current tile and target tile
				distances = Vector2i::Distance(pos + ApplyMovementI((Movement)i, TILE_SIZE), targetPos);

				//if move is valid
				if (moves[i] != Movement::mSTOP) {
					//excluding backwards
					if (moves[i] != backwards) {
						//if distance is shorter than last stored
						if (distances < nearest) {
							//store smaller distance
							nearest = distances;

							//store accompanying move
							result = moves[i];
						}
					}
				}
			}
		}
		break;
	}

	//DEBUG
	if (debugFlag) {
		for (int i = 0; i < 4; i++) {
			cout << "move " << i << " : " << (int)moves[i] << endl;
		}

		debugFlag = false;
	}

	//return move result
	return result;
}

void GameManager::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey) {
	// Checks if ESCAPE key is pressed
	if (state->IsKeyDown(Input::Keys::ESCAPE)) {
		Graphics::Destroy();
	}

	switch (_currentState) {
	case GameState::gsStarting:
		_status->ConfigState(false);
		// Checks is StartGame key is pressed (Start Screen)
		if (state->IsKeyDown(startKey)) {
			_currentState = GameState::gsPlaying;
		}
		break;
	case GameState::gsPlaying:
		// Checks if Pause key is pressed (Pause Screen)
		if (!_status->pauseKeyDown && state->IsKeyDown(pauseKey)) {
			_status->ConfigState(true);
			_currentState = GameState::gsPausing;
		
			_status->pauseKeyDown = true;
		}
		break;
	case GameState::gsPausing:
		if (!_status->pauseKeyDown && state->IsKeyDown(pauseKey)) {
			_currentState = GameState::gsPlaying;
	
			_status->pauseKeyDown = true;
		}
		break;
	default:
		break;
	}

	// Reset Pause Key Flag
	if (state->IsKeyUp(pauseKey)) {
		_status->pauseKeyDown = false;
	}

	// Reset Start Key Flag
	if (state->IsKeyUp(pauseKey)) {
		_status->startKeyDown = false;
	}
}

void GameManager::CheckViewportCollision(Rect* pos) {
	/* wrap-around */
	if (pos->Y < 0.0f) {
		pos->Y = WHEIGHT - pos->Height;
	}

	if (pos->Y > WHEIGHT - pos->Height) {
		pos->Y = 0.0f;
	}

	if (pos->X < 0.0f) {
		pos->X = WWIDTH - pos->Width;
	}

	if (pos->X > WWIDTH - pos->Width) {
		pos->X = 0.0f;
	}
}

void GameManager::DefineMap() {
	int munchieCounter = 0;
	int maxMunchies = _level->CountTiles(TileType::tMUNCHIE);	//counts all munchies including power pellets

	int wallCounter = 0;
	int maxWalls = _level->CountTiles(TileType::tWALL);		//counts all walls

	int ghostCounter = 0;
	Vector2i tmpPos;

	TileType target;

	for (int row = 0; row < _level->MaxRows(); row++) {
		for (int col = 0; col < _level->MaxCols(); col++) {
			target = _level->GetTileType(row, col);

			switch (target) {
			case TileType::tPLAYER:
				_pacman->position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					TILE_SIZE, TILE_SIZE
				);
				break;
			case TileType::tPELLET:
			case TileType::tMUNCHIE:
				//state
				_munchies[munchieCounter].isEaten = false;
				
				if (target == TileType::tPELLET) {
					_munchies[munchieCounter].isPowerPellet = true;
					_munchies[munchieCounter].value = 50;
					_munchies[munchieCounter].scale = 2.0f;
				} else {
					_munchies[munchieCounter].isPowerPellet = false;
					_munchies[munchieCounter].value = 10;
					_munchies[munchieCounter].scale = 1.0f;
				}

				//size
				_munchies[munchieCounter].offset =
					(MUNCHIE_SIZE * _munchies[munchieCounter].scale) / _munchies[munchieCounter].scale;
				
				//position
				_munchies[munchieCounter].position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					MUNCHIE_SIZE,
					MUNCHIE_SIZE
				);
				
				if (munchieCounter < maxMunchies) {
					munchieCounter++;
				}
				break;

			case TileType::tWALL:
				//state
				_walls[wallCounter].canGhostsPass = false;
				_walls[wallCounter].canGhostsSpawn = false;
				
				_walls[wallCounter].canPlayerPass = false;
				_walls[wallCounter].canPlayerSpawn = false;
				
				_walls[wallCounter].canBonusSpawn = false;

				_walls[wallCounter].mapCoords = Vector2i(col, row);

				//position
				_walls[wallCounter].position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					TILE_SIZE, TILE_SIZE
				);

				if (wallCounter < maxWalls) {
					wallCounter++;
				}
				break;
			case TileType::tBONUS:
				_fruit->position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					TILE_SIZE, TILE_SIZE
				);
				break;
			case TileType::tGHOST:
				//position
				_ghosts[ghostCounter].position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					TILE_SIZE, TILE_SIZE
				);
				
				if (ghostCounter < NUM_OF_GHOSTS) {
					ghostCounter++;
				}
				break;
			}
		}
	}

	//add additional ghosts, if any
	while (ghostCounter < NUM_OF_GHOSTS) {
		tmpPos = _level->GetRandomTilePos(TileType::tGHOST);

		_ghosts[ghostCounter].position = new Rect(
			tmpPos.X * TILE_SIZE,
			tmpPos.Y * TILE_SIZE,
			TILE_SIZE, TILE_SIZE
		);

		ghostCounter++;
	}
}

void GameManager::Draw(int elapsedTime) {
	SpriteBatch::BeginDraw(); // Starts Drawing

	/* Collectables */

	// Draw Munchies
	for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
		_munchies[i].Draw(_frameCount);
	}

	// Draw Fruits
	_fruit->Draw(_frameCount);

	/* Players */

	// Draw PacMan
	_pacman->Draw(_frameCount);

	// Draw Ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		_ghosts[i].Draw(_frameCount);
	}

	/* Obstacles */

	// Draw Walls
	for (int i = 0; i < _level->CountTiles(TileType::tWALL); i++) {
		_walls[i].Draw();
	}

	/* HUD */
	_hud->Draw();

	// Manage Animations
	// Reset animation loop
	if (_frameCount >= 60) {
		_frameCount = 0;
	}

	switch(_currentState) {
	case GameState::gsStarting:
	case GameState::gsPausing:
		// Draw Start and Pause screens
		_status->Draw();
		break;
	case GameState::gsPlaying:
		// Increment animation counter
		_frameCount++;
		break;
	default:
		break;
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

Movement GameManager::ChaseMotion(Vector2i obj, Vector2i git) {
	//obj = my pos
	//git = target pos
	// counter to check number of moves available
	int checkMoves = 0;

	// result to return
	Movement result = Movement::mSTOP;

	// array of movement choices available
	Movement moves[4] = {
		Movement::mSTOP, Movement::mSTOP,
		Movement::mSTOP, Movement::mSTOP
	};

	Vector2i tmp;
	int distances[4];
	for (int i = 0; i < 4; i++) {
		tmp = obj + ApplyMovementI((Movement)i, TILE_SIZE);

		distances[i]= Vector2i::Distance(tmp, git);
	}

	// fill array with available movements
	Vector2i pos = obj / TILE_SIZE;

	moves[0] = _level->IsMapTile(pos.Y, pos.X - 1, TileType::tWALL) ? Movement::mSTOP : Movement::mLEFT;
	moves[1] = _level->IsMapTile(pos.Y, pos.X + 1, TileType::tWALL) ? Movement::mSTOP : Movement::mRIGHT;
	moves[2] = _level->IsMapTile(pos.Y - 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mUP;
	moves[3] = _level->IsMapTile(pos.Y + 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mDOWN;

	int shortestDistance = 9999999999;
	Movement shortestDirection = Movement::mSTOP;
	for (int i = 0; i < 4; i++) {
		if (moves[i] != Movement::mSTOP) {
			if (distances[i] < shortestDistance) {
				shortestDistance = distances[i];
				shortestDirection = moves[i];
			}
		}
	}

	return shortestDirection;

	// count available movements
	for (int i = 0; i < 4; i++) {
		if (moves[i] != Movement::mSTOP) {
			checkMoves++;
		}
	}

	// if more than one movement available...
	if (checkMoves > 1) {
		// randomly choose an available move
		while (result == Movement::mSTOP) {
			result = moves[rand() % 4];
		}
	} else {
		//cout << "Ghost at " << pos.X << " x " << pos.Y <<
		//	"\nOnly ONE possible direction?!" << endl;
	}

	// return chosen move
	return result;
}

Movement GameManager::ForwardMotion(Vector2i pos, Movement currMove){
	// counter to check number of moves available
	int checkMoves = 0;							

	// result to return
	Movement result = Movement::mSTOP;

	// array of movement choices available
	Movement moves[4] = {						
		Movement::mSTOP, Movement::mSTOP,
		Movement::mSTOP, Movement::mSTOP
	};

	// fill array with available movements
	moves[0] = _level->IsMapTile(pos.Y, pos.X - 1, TileType::tWALL) ? Movement::mSTOP : Movement::mLEFT;
	moves[1] = _level->IsMapTile(pos.Y, pos.X + 1, TileType::tWALL) ? Movement::mSTOP : Movement::mRIGHT;
	moves[2] = _level->IsMapTile(pos.Y - 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mUP;
	moves[3] = _level->IsMapTile(pos.Y + 1, pos.X, TileType::tWALL) ? Movement::mSTOP : Movement::mDOWN;

	// count available movements
	for (int i = 0; i < 4; i++) {
		if (moves[i] != Movement::mSTOP) {
			checkMoves++;

			// is current move still valid
			if (moves[i] == currMove) {
				//earmark it for later use
				result = currMove;
			}
		}
	}

	// if ONLY two moves are available...
	if (checkMoves == 2) {
		/*
		one move is the direction this ghost came from.
		the other move is either:
			a) the direction the ghost was already heading
			- or -
			b) the only other direction available WITHOUT going backwards
		*/

		//case a = if current move was valid, then we already have it stored
		if (result == currMove) {
		} else {
			//case b = find the remaining valid move
			Movement backwards = Movement();

			//loop through stored moves
			for (int i = 0; i < 4; i++) {
				if (moves[i] != Movement::mSTOP) {
					//work out the backwards movement
					switch (currMove) {
					case Movement::mLEFT:
						backwards = Movement::mRIGHT;
						break;
					case Movement::mRIGHT:
						backwards = Movement::mLEFT;
						break;
					case Movement::mUP:
						backwards = Movement::mDOWN;
						break;
					case Movement::mDOWN:
						backwards = Movement::mUP;
						break;
					default:
						break;
					}

					//ensure backwards movement is not selected
					if (moves[i] != backwards) {
						result = moves[i];
						//more efficient to break rather than fall out of loop
						break;
					}
				}
			}
		}
	}
	/*
	more (or less) than one move available, so stop.
	if less than two moves available then there's an error.
	otherwise, stopping will force a new move to be chosen
	*/

	// return chosen move
	return result;
}

bool GameManager::HasTargetHitObject(Rect* git, Rect* obj, float tolerance, char mode) {
	float slim = tolerance;
	float gitRadiusX = (git->Width - slim) / 2;
	float gitRadiusY = (git->Height - slim) / 2;
	float gitCenterX = git->X + (gitRadiusX / 2) + (slim / 2);
	float gitCenterY = git->Y + (gitRadiusY / 2) + (slim / 2);

	//find center and radius of obj
	float objRadiusX = obj->Width / 2;
	float objRadiusY = obj->Height / 2;
	float objCenterX = obj->X + objRadiusX;
	float objCenterY = obj->Y + objRadiusY;

	float dist = sqrt(
		pow(gitCenterX - objCenterX, 2) +
		pow(gitCenterY - objCenterY, 2)
	);

	//NOTE: all sprites are square. width = height
	// so just use one for simplicity
	float range = gitRadiusX + objRadiusX;

	//box collision adjustment
	Rect tmpTarget = Rect(
		git->X + (tolerance / 2),
		git->Y + (tolerance / 2),
		git->Width - tolerance,
		git->Height - tolerance
	);

	switch (mode) {
		// Box (rectangle) Collision
	case 'b':
	case 'B':
		//test for non-collisions
		//if ((btm1 >= top2) && (top1 <= btm2) && (left1 <= right2) && (right1 >= left2))
		if (
			//top1 < bot2
			(git->Y < obj->Y + obj->Height) &&
			//bot1 > top2
			(git->Y + git->Height > obj->Y) &&
			//left1 < right2
			(git->X < obj->X + obj->Width) &&
			//right1 > left2
			(git->X + git->Width > obj->X)
			) {
			//at this point, there must have been a collision
			return true;
		}
		else {
			return false;
		}
		break;

		// Circle Collision
	case 'c':
	case 'C':
		//test distance of center positions
		if (dist < range) {
			return true;
		}
		else {
			return false;
		}
		break;
	default:
		return false;
		break;
	}
}

void GameManager::InitialiseGhosts() {
	int ghostID = 0;
	srand(time(0));

	//load textures
	_ghostTexture->Load("Textures/Ghosts.png", true);
	
	//loop through ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		//instatiate
		_ghosts[i] = Enemy();

		//allocate texture of ghost
		_ghosts[i].texture = _ghostTexture;

		//set animation frames
		_ghosts[i].SetMaxFrames(2);
		
		//choose ghost persona
		_ghosts[i].persona = (GhostType)ghostID;
		_ghosts[i].sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);

		//set offset
		_ghosts[i].SetOffset(TILE_SIZE / 2);
		
		//choose next ghost texture
		if (ghostID < 3) {
			ghostID++;
		} else {
			//randomise ghosts after the first 4
			ghostID = rand() % 4;
		}
	}
}

void GameManager::InitialiseMunchies() {
	_munchieTexture->Load("Textures/Munchie.png", true);

	for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
		_munchies[i] = Munchie(MUNCHIE_SIZE);
		_munchies[i].SetTexture(_munchieTexture);
		_munchies[i].sourceRect = new Rect(0, 0, MUNCHIE_SIZE, MUNCHIE_SIZE);
		_munchies[i].SetFrames(MUNCHIE_SIZE);		
		_munchies[i].isEaten = false;
		_munchies[i].isPowerPellet = false;
		_munchies[i].value = 0;
		_munchies[i].scale = 0;
		_munchies[i].SetOffset(TILE_SIZE / 2);
	}
}

void GameManager::InitialiseWalls() {
	_wallTexture->Load("Textures/WallTile.png", true);

	for (int i = 0; i < _level->CountTiles(TileType::tWALL); i++) {
		_walls[i] = Obstacle();
		_walls[i].SetTexture(_wallTexture);
		_walls[i].SetOffset(TILE_SIZE / 2);
		_walls[i].sourceRect = new Rect(0.0f, 0.0f, TILE_SIZE, TILE_SIZE);

		_walls[i].canBonusSpawn = false;
		_walls[i].canGhostsPass = false;
		_walls[i].canGhostsSpawn = false;
		_walls[i].canPlayerPass = false;
		_walls[i].canPlayerSpawn = false;
	}
}

void GameManager::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState) {
	// Gets the current state of the keyboard
	if (_currentState != GameState::gsPausing) {
		
		// Checks if W key is pressed
		if (state->IsKeyDown(Input::Keys::W) ||
			state->IsKeyDown(Input::Keys::UP)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mUP;
		}

		// Checks if S key is pressed
		else if (state->IsKeyDown(Input::Keys::S) ||
			state->IsKeyDown(Input::Keys::DOWN)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mDOWN;
		}

		// Checks if A key is pressed
		else if (state->IsKeyDown(Input::Keys::A) ||
			state->IsKeyDown(Input::Keys::LEFT)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mLEFT;
		}

		// Checks if D key is pressed
		else if (state->IsKeyDown(Input::Keys::D) ||
			state->IsKeyDown(Input::Keys::RIGHT)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mRIGHT;
		}
		

		// Checks if CTRL key is pressed
		else if (state->IsKeyDown(Input::Keys::LEFTCONTROL)) {
			_pacman->StartBoosting();
		}
	
		/*DEBUG*/
		// check for [ENTER]
		else if (state->IsKeyDown(Input::Keys::RETURN)) {
			debugFlag = true;
		}

		// Gets the current state of the left mouse button
		if (mouseState->LeftButton == Input::ButtonState::PRESSED) {
			//DEBUG
			//cout << "next move is > " << static_cast<int>(_pacman->nextMove) << endl;
			Vector2i mTile;
			mTile.X = floor(mouseState->X / TILE_SIZE);
			mTile.Y = floor(mouseState->Y / TILE_SIZE);
			/*
			cout << "Mouse LeftClick at: X-" << mouseState->X << ", Y-" << mouseState->Y <<
				"\n(maptile @ map[" << mTile.Y << "][" << mTile.X << "] = " <<
				map[mTile.Y][mTile.X] << ")";
			*/

		}
		/*
		// Gets the current state of right mouse button
		if (mouseState->RightButton == Input::ButtonState::PRESSED) {
			//to be refactored into a function
			if (!_fruits->isShown) {
				SpawnFruit();
			}
		}
		*/
	}
}

bool GameManager::IsPassable(Vector2i pos, Movement move, bool isPlayer) {
	pos += ApplyMovementI(move, 1);
	
	// test map-tile coords
	if (move != Movement::mSTOP) {
		// if next tile is not a wall ...
		if (_level->GetTileType(pos.Y, pos.X) != TileType::tWALL) {
			if (isPlayer) {
				//... and not the entrance to ghosts home...
				if (!_level->IsMapTile(pos.Y, pos.X, TileType::tGATE)) {
					// passable tile found
					return true;
				}
			} else {	//not player (aka ghosts)
				//don't return home unless dead/been munched
				return true;
			}
		}
	}

	return false;
}

void GameManager::LoadContent() {
	/* Players */

	// PacMan (the player)
	_pacman->texture->Load("Textures/Pacman.png", false);

	// Ghosts
	InitialiseGhosts();

	/* Collectables */

	// Load Fruits
	_fruit->texture->Load("Textures/BonusV2.png", true);
	_fruit->SetFrames(_fruit->texture->GetWidth());

	// Load Munchies
	InitialiseMunchies();

	/* Obstacles */

	// Load Walls
	InitialiseWalls();

	DefineMap();

	/* Music & Sound Effects */
	// eat a munchie
	_pop->Load("Sounds/pop.wav");

	// game intro
	_intro->Load("Sounds/pacman_beginning.wav");

	// pacman's death
	_death->Load("Sounds/pacman_death.wav");

	// eating a bonus
	_eatFruit->Load("Sounds/pacman_eatfruit.wav");

	// eating a ghost
	_eatGhost->Load("Sounds/pacman_eatghost.wav");

	// eating all the munchies
	_nextLevel->Load("Sounds/pacman_intermission.wav");

	/* HUD */
	_status->texture->Load("Textures/Transparency.png", true);
	_status->title->Load("Textures/PacManText.png", true);
}

Movement GameManager::NotSoRandomMotion(Vector2i pos) {
	Movement result = Movement::mSTOP;
	Movement moves[4] = {						// array of movement choices available
		Movement::mSTOP, Movement::mSTOP,
		Movement::mSTOP, Movement::mSTOP
	};

	int checkMoves = 0;							// counter to check number of moves available
	int row = floor(pos.Y / TILE_SIZE);
	int col = floor(pos.X / TILE_SIZE);

	// fill array with available movements
	moves[0] = _level->IsMapTile(row, col - 1, TileType::tWALL) ? Movement::mSTOP : Movement::mLEFT;
	moves[1] = _level->IsMapTile(row, col + 1, TileType::tWALL) ? Movement::mSTOP : Movement::mRIGHT;
	moves[2] = _level->IsMapTile(row - 1, col, TileType::tWALL) ? Movement::mSTOP : Movement::mUP;
	moves[3] = _level->IsMapTile(row + 1, col, TileType::tWALL) ? Movement::mSTOP : Movement::mDOWN;

	// count available movements
	for (int i = 0; i < 4; i++) {
		if (moves[i] != Movement::mSTOP) {
			checkMoves++;
		}
	}

	if (checkMoves == 0) {
		//DEBUG
		cout << "No moves found!" << endl;
	} else {
		if (debugFlag) {
			for (int i = 0; i < 4; i++) {
				cout << "move " << i << " : " << (int)moves[i] << endl;
			}
			debugFlag = false;
		}
	}


	// if more than one movement available...
	if (checkMoves > 1) {
		// randomly choose an available move
		while (result == Movement::mSTOP) {
			result = moves[rand() % 4];
		}
	}
	else {
		//cout << "Ghost at " << pos.X << " x " << pos.Y <<
		//	"\nOnly ONE possible direction?!" << endl;
	}

	// return chosen move
	return result;
}

void GameManager::ResetGame(bool nextLevel) {
	if (nextLevel) {
		// update score
		score += 100 * _level->levelCounter;

		// play sound
		Audio::Play(_nextLevel);

	} else {
		// update score
		score = 0;
	}

	DefineMap();
}

void GameManager::ResetGhosts() {
	Vector2i tmpPos = *Vector2i::Zero;
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		_ghosts[i].SetPosInt(_level->GetRandomTilePos(TileType::tGHOST) * TILE_SIZE);
		_ghosts[i].isChasing = true;
	}
}

void GameManager::ResetPlayer() {
	_pacman->SetPosInt(_level->GetRandomTilePos(TileType::tPLAYER) * TILE_SIZE);
	_pacman->currMove = Movement::mSTOP;
	_pacman->nextMove = Movement::mSTOP;
}

void GameManager::ScareGhosts() {
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		_ghosts[i].StartFlapping();
		/*
		if (_ghostFearTimer < 1) {
			_ghostFearTimer = _ghostFearTimerMax;

			for (int i = 0; i < NUM_OF_GHOSTS; i++) {
				_ghosts[i]->isChasing = false;
				_ghosts[i]->isEatable = true;
			}
		}
		*/
	}
}

void GameManager::Update(int elapsedTime) {
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();
	CheckPaused(keyboardState, Input::Keys::P, Input::Keys::SPACE);

	Input(elapsedTime, keyboardState, mouseState);

	if (_pacman->lives < 0) { _currentState = GameState::gsStopping; }

	switch (_currentState) {
	case GameState::gsStarting:
		// play intro once AND stop
		if (!introPlayed) {
			if(_intro->GetState() != SoundEffectState::PLAYING){
				Audio::Play(_intro);
				introPlayed = true;

				_hud->LoadScores();
			}
		}
		break;
	case GameState::gsPlaying:
			/* Collectables */

			// Update Fruits
			_fruit->Update(_frameCount);

			// Update Munchies
			UpdateMunchies();

			/* Players */

			// Update Pacman
			UpdatePacman(elapsedTime);

			// Update Ghosts
			UpdateGhosts(elapsedTime);

			/* Checks */
			CheckCollisions();
			break;
	case GameState::gsDeath:
		if (!_pacman->DeathSequence()) {
			ResetGhosts();
			ResetPlayer();
			_currentState = GameState::gsPlaying;
		}
		break;
	case GameState::gsStopping:
		_hud->AddScore(_hud->GetInput("", score));
		_hud->SortScores();
		break;
	}

	_hud->CurrentScore(score);
}

void GameManager::UpdateGhosts(int elapsedTime) {
	Vector2 tmpDir = Vector2();
	Vector2i targetPos = Vector2i();
	Vector2i ghostPos = Vector2i();
	float tmpSpeed = 0;

	for (int i = 0; i < NUM_OF_GHOSTS; i++ ) {
		//debug
		if (debugFlag) {
			cout << "DEBUGGING UpdateGhosts(int elapsedTime):" << endl;
		}

		//perform individual update
		_ghosts[i].Update(elapsedTime);

		//get individual ghost speed
		tmpSpeed = _ghosts[i].GetSpeed() * elapsedTime;

		//get ghosts' pos, rounded
		ghostPos = *_ghosts[i].GetPosInt();

		//if ghost is near center of tile
		if ((ghostPos.X % TILE_SIZE < 1) && 
			(ghostPos.Y % TILE_SIZE < 1)) {

			// re-align to tile grid
			_ghosts[i].position->X = (floor(_ghosts[i].position->X / TILE_SIZE) * TILE_SIZE);
			_ghosts[i].position->Y = (floor(_ghosts[i].position->Y / TILE_SIZE) * TILE_SIZE);

			//choose a movement
			_ghosts[i].nextMove = CheckMotion(ghostPos, _ghosts[i].nextMove, _ghosts[i].persona);

			//force a movement
			switch (_level->GetTileType(ghostPos.Y / TILE_SIZE, ghostPos.X / TILE_SIZE)) {
			case TileType::tUP:
			case TileType::tGATE:
				//path out of base
				_ghosts[i].nextMove = Movement::mUP;
				break;
			case TileType::tLEFT:
				//left warp
				ghostPos = _level->GetRandomTilePos(TileType::tRIGHT) * TILE_SIZE;
				ghostPos.X -= TILE_SIZE;
				_ghosts[i].SetPosInt(ghostPos);
				break;
			case TileType::tRIGHT:
				//right warp
				ghostPos = _level->GetRandomTilePos(TileType::tLEFT) * TILE_SIZE;
				ghostPos.X += TILE_SIZE;
				_ghosts[i].SetPosInt(ghostPos);
				break;
			default:
				
				break;
			}

			//set nextMove to currentMove
			_ghosts[i].currMove = _ghosts[i].nextMove;
		}

		// check for map collisions
		tmpDir = ApplyMovementF(_ghosts[i].currMove, tmpSpeed);
		if(_level->IsMapTile(
			floor(ghostPos.Y + tmpDir.Y) / TILE_SIZE,
			floor(ghostPos.X + tmpDir.X) / TILE_SIZE,
			TileType::tWALL)) {

			// stop movement
			_ghosts[i].currMove = Movement::mSTOP;
		}

		if (_ghosts[i].currMove != Movement::mSTOP) {
			// apply tested direction to pacman's direction
			_ghosts[i].MoveBy(tmpDir);
		} 

		// constrain to viewport
		CheckViewportCollision(_ghosts[i].position);
	}
}

void GameManager::UpdateMunchies() {
	int munchieCounter = 0;
	for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
		_munchies[i].Update(_frameCount);

		// count the eaten munchies
		if (_munchies[i].isEaten) {
			munchieCounter++;
		}
	}

	// if all munchies have been eaten...
	if (munchieCounter >= _level->CountTiles(TileType::tMUNCHIE)) {
		ResetGame(true);
	}
}

void GameManager::UpdatePacman(int elapsedTime) {
	_pacman->Update(elapsedTime);

	Rect tmpRect = Rect();
	Vector2 tmpDir = Vector2();
	float tmpSpeed = _pacman->GetSpeed() * elapsedTime;

	//get player pos, rounded
	Vector2i tmpPos = *_pacman->GetPosInt();
	
	//if player is near center of tile
	if ((tmpPos.X % TILE_SIZE < tmpSpeed) &&
		(tmpPos.Y % TILE_SIZE < tmpSpeed)) {

		//if nextMove is valid...
		if (IsPassable(tmpPos / TILE_SIZE , _pacman->nextMove, true)) {
			//...update currMove
			_pacman->currMove = _pacman->nextMove;
		}

		//if currMove is NOT valid
		if (!IsPassable(tmpPos / TILE_SIZE, _pacman->currMove, true)) {
			// re-align to tile grid
			_pacman->position->X = (floor(tmpPos.X / TILE_SIZE) * TILE_SIZE);
			_pacman->position->Y = (floor(tmpPos.Y / TILE_SIZE) * TILE_SIZE);
		
			// halt
			_pacman->currMove = Movement::mSTOP;
		}

		//if warp tile
		if (_level->GetTileType(tmpPos.Y / TILE_SIZE, tmpPos.X / TILE_SIZE) == TileType::tLEFT) {
			tmpPos = _level->GetRandomTilePos(TileType::tRIGHT) * TILE_SIZE;
			tmpPos.X -= TILE_SIZE;
			_pacman->SetPosInt(tmpPos);
		} else if(_level->GetTileType(tmpPos.Y / TILE_SIZE, tmpPos.X / TILE_SIZE) == TileType::tRIGHT){
			tmpPos = _level->GetRandomTilePos(TileType::tLEFT) * TILE_SIZE;
			tmpPos.X += TILE_SIZE;
			_pacman->SetPosInt(tmpPos);
		}
	}

	// apply tested direction to pacman's direction
	_pacman->MoveBy(ApplyMovementF(_pacman->currMove, tmpSpeed));

	// constrain to viewport
	CheckViewportCollision(_pacman->position);
	
}