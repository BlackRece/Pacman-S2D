#include "GameManager.h"

using namespace std;

GameManager::GameManager(int argc, char* argv[]) :
	Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(30)
{
	_frameCount = 0;
	_time = 0;
	_currentState = GameState::gsPlaying;

	// initialise level
	_level = new LevelManager();

	/* PacMan */

	// initialise player
	_pacman = new Player();
	_pacman->SetMaxFrames(2);
	_pacman->SetSpeed(_cPacmanSpeed);
	_pacmanTexture = new Texture2D();
	_pacman->sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
	_pacman->SetFrameDelay(_cPacmanFrameTime);
	_pacman->SetOffset(TILE_SIZE / 2);

	/* Collectables */

	// initialise munchies
	_munchies = new Munchie[_level->CountTiles(TileType::tMUNCHIE)];
	_munchieTexture = new Texture2D();
	_munchieCount = 0;

	// initialise fruits
	_fruit = new Bonus();
	_fruit->maxNum = 8;
	_fruitTexture = new Texture2D();
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

	/* HUD */
	score = 0;

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, WWIDTH, WHEIGHT, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

GameManager::~GameManager() {
	/* Textures */
	delete _fruitTexture;
	delete _munchieTexture;
	delete _pacmanTexture;
	delete _wallTexture;

	/* Audio */
	delete _pop;
	delete _intro;
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
			this->score += _fruit->GetValue();

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
				this->score += _munchies[i].value;

				//play soundfx
				Audio::Play(_pop);

				//scare ghosts!
				if (_munchies[i].isPowerPellet) {
					//ScareGhosts();
				}
			}
		}
	}
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
		} else {
			return false;
		}
		break;

		// Circle Collision
	case 'c':
	case 'C':
		//test distance of center positions
		if (dist < range) {
			return true;
		} else {
			return false;
		}
		break;
	default:
		return false;
		break;
	}
}

void GameManager::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey) {
	// Checks if ESCAPE key is pressed
	if (state->IsKeyDown(Input::Keys::ESCAPE)) {
		Graphics::Destroy();
	}

	// Checks is StartGame key is pressed (Start Screen)
	if ((state->IsKeyDown(startKey)) && (_currentState == GameState::gsStarting)) {
		_currentState = GameState::gsPlaying;
		Audio::Play(_intro);
	}

	// Checks if Pause key is pressed (Pause Screen)
	if (_currentState == GameState::gsPausing && state->IsKeyDown(pauseKey)) {
		_currentState = GameState::gsPlaying;
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
				//texture
				_munchies[munchieCounter].sourceRect = new Rect(0, 0, MUNCHIE_SIZE, MUNCHIE_SIZE);

				//state
				_munchies[munchieCounter].isEaten = false;
				
				if (target == TileType::tPELLET) {
					_munchies[munchieCounter].isPowerPellet = true;
					_munchies[munchieCounter].value = 50;
					_munchies[munchieCounter].scale = 2.0f;
					_munchies[munchieCounter].offset = 0;
				} else {
					_munchies[munchieCounter].isPowerPellet = false;
					_munchies[munchieCounter].value = 10;
					_munchies[munchieCounter].scale = 1.0f;
					_munchies[munchieCounter].offset = TILE_SIZE / 4;
				}

				//position
				_munchies[munchieCounter].position = new Rect(
					(col * TILE_SIZE),
					(row * TILE_SIZE),
					MUNCHIE_SIZE * _munchies[munchieCounter].scale,
					MUNCHIE_SIZE * _munchies[munchieCounter].scale
				);
				
				if (munchieCounter < maxMunchies) {
					munchieCounter++;
				}
				break;

			case TileType::tWALL:
				//texture
				_walls[wallCounter].sourceRect = new Rect(0.0f, 0.0f, TILE_SIZE, TILE_SIZE);

				//state
				_walls[wallCounter].canGhostsPass = false;
				_walls[wallCounter].canGhostsSpawn = false;
				
				_walls[wallCounter].canPlayerPass = false;
				_walls[wallCounter].canPlayerSpawn = false;
				
				_walls[wallCounter].canBonusSpawn = false;

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
			}
		}
	}
}

void GameManager::Draw(int elapsedTime) {
	SpriteBatch::BeginDraw(); // Starts Drawing

	/* Players */

	// Draw PacMan
	_pacman->Draw(_frameCount);

	/* Collectables */

	// Draw Munchies
	for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
		_munchies[i].Draw(_frameCount);
	}

	// Draw Fruits
	_fruit->Draw(_frameCount);

	/* Obstacles */

	// Draw Walls
	for (int i = 0; i < _level->CountTiles(TileType::tWALL); i++) {
		_walls[i].Draw();
	}

	// Manage Animations
	// Reset animation loop
	if (_frameCount >= 60) {
		_frameCount = 0;
	}

	switch(_currentState) {
	case GameState::gsPlaying:
		_frameCount++;
		break;
	default:
		break;
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

void GameManager::LoadContent() {
	/* Players */

	// PacMan (the player)
	_pacmanTexture->Load("Textures/Pacman.tga", false);
	_pacman->SetTexture(_pacmanTexture);

	/* Collectables */

	// Load Fruits
	_fruitTexture->Load("Textures/BonusV2.png", true);
	_fruit->SetTexture(_fruitTexture);
	_fruit->SetFrames(_fruitTexture->GetWidth());

	// Load Munchies
	_munchieTexture->Load("Textures/Munchie.png", true);
	InitialiseMunchies(_level->CountTiles(TileType::tMUNCHIE));

	/* Obstacles */

	// Load Walls
	_wallTexture->Load("Textures/WallTile.png", true);
	InitialiseWalls(_level->CountTiles(TileType::tWALL));

	DefineMap();

	/* Music & Sound Effects */
	// eat a munchie
	_pop->Load("Sounds/pop.wav");

	// game intro
	_intro->Load("Sounds/pacman_beginning.wav");

	_death->Load("Sounds/pacman_death.wav");
	_eatFruit->Load("Sounds/pacman_eatfruit.wav");
	_eatGhost->Load("Sounds/pacman_eatghost.wav");
	_nextLevel->Load("Sounds/pacman_intermission.wav");
}

void GameManager::InitialiseMunchies(int numOfMunchies) {
	if (numOfMunchies < 1) {
		_munchieCount = NUM_OF_MUNCHIES;
	} else {
		_munchieCount = numOfMunchies;
	}

	
	for (int i = 0; i < _munchieCount; i++) {
		//all munchies
		_munchies[i] = Munchie(MUNCHIE_SIZE);
		_munchies[i].SetTexture(_munchieTexture);
		_munchies[i].SetFrames(MUNCHIE_SIZE);		
		_munchies[i].isEaten = false;
		_munchies[i].isPowerPellet = false;
		_munchies[i].value = 0;
		_munchies[i].scale = 0;
		_munchies[i].SetOffset(TILE_SIZE / 2);
	}
}

void GameManager::InitialiseWalls(int numOfWalls) {
	if (numOfWalls > 0) {
		for (int i = 0; i < numOfWalls; i++) {
			_walls[i] = Obstacle();
			_walls[i].SetTexture(_wallTexture);
			_walls[i].SetOffset(TILE_SIZE / 2);

			_walls[i].canBonusSpawn = false;
			_walls[i].canGhostsPass = false;
			_walls[i].canGhostsSpawn = false;
			_walls[i].canPlayerPass = false;
			_walls[i].canPlayerSpawn = false;
		}
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
	pos += this->ApplyMovementI(move, 1);
	
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

void GameManager::Update(int elapsedTime) {
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();
	CheckPaused(keyboardState, Input::Keys::P, Input::Keys::SPACE);

	Input(elapsedTime, keyboardState, mouseState);

	/* Collectables */

	// Update Fruits
	_fruit->Update(_frameCount);

	// Update Munchies
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
		/* Reset Level */
		// update score
		this->score += 100 * _level->levelCounter;

		// play sound
		Audio::Play(_nextLevel);
		
		// reset munchies
		for (int i = 0; i < _level->CountTiles(TileType::tMUNCHIE); i++) {
			_munchies[i].isEaten = false;
		}

		// reset ghosts

		// reset fruit
		_fruit->SetPosition(_level->GetRandomTilePos(TileType::tBONUS) * TILE_SIZE);

		// reset pacman
		_pacman->nextMove = Movement::mSTOP;
		_pacman->currMove = Movement::mSTOP;
		_pacman->SetPosInt(_level->GetRandomTilePos(TileType::tPLAYER) * TILE_SIZE);

	}

	/* Players */

	// Update Pacman
	this->UpdatePacman(elapsedTime);

	// Update Ghosts

	/* Checks */
	CheckCollisions();

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
		if (this->IsPassable(tmpPos / TILE_SIZE , _pacman->nextMove, true)) {
			//...update currMove
			_pacman->currMove = _pacman->nextMove;
		}

		//if currMove is NOT valid
		if (!this->IsPassable(tmpPos / TILE_SIZE, _pacman->currMove, true)) {
			// re-align to tile grid
			_pacman->position->X = (floor(tmpPos.X / TILE_SIZE) * TILE_SIZE);
			_pacman->position->Y = (floor(tmpPos.Y / TILE_SIZE) * TILE_SIZE);
		
			// halt
			_pacman->currMove = Movement::mSTOP;
		}

	}

	// apply tested direction to pacman's direction
	_pacman->MoveBy(ApplyMovementF(_pacman->currMove, tmpSpeed));

	// constrain to viewport
	CheckViewportCollision(_pacman->position);
}