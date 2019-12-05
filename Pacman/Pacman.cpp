#include "Pacman.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

Pacman::Pacman(int argc, char* argv[]) : 
	Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250)
{
	_frameCount = 0;
	_pacmanStartPos = Vector2i(350, 350);

	// initialise pacman/player
	_pacman = new Player();
	_pacman->currentFrametime = 0;
	_pacman->frame = 0;
	_pacman->texture = new Texture2D();
	_pacman->position = new Vector2(_pacmanStartPos.get());
	_pacman->lives = 3;
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->direction = new Vector2(0.0f, 0.0f);
	_pacman->currMove = Movement::mSTOP;
	_pacman->nextMove = Movement::mSTOP;
	_pacman->lastMove = Movement::mSTOP;
	_pacman->facing = Direction::dLEFT;
	_pacman->speed = _cPacmanSpeed;
	_pacman->isBoosting = false;
	_pacman->boostDuration = 10;
	_pacman->boostDurationMax = 10;
	_pacman->boostMultiply = 2;
	_pacman->score = 0;

	// initialise walls
	_wallTexture = new Texture2D();
	_wallScale = 1.0f;
	
	// Collectables
	// initialise munchies
	_munchieTexture = new Texture2D();
	
	// initialise fruits
	_fruitTexture = new Texture2D();
	_fruits = new Bonus();
	_fruits->self = Entity();
	_fruits->self.texture = new Texture2D();
	_fruits->self.sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
	_fruits->self.position = new Rect();
	_fruits->isShown = false;
	_fruits->spawnMaxTime = 200;
	_fruits->spawnTimer = 0;
	_fruits->spawnDelay = _fruits->spawnMaxTime / 2;
	_fruits->value = 100;
	_fruits->num = 0;
	_fruits->maxNum = 8;
	_fruits->curFrames = 0;
	_fruits->maxFrames = 2;	

	// initialise power ups
	_powerUp = new Bonus();
	_powerUp->self.texture = new Texture2D();
	//_powerTexture = new Texture2D();

	// initialise ghosts
	_ghostTexture = new Texture2D();
	_ghostFearTimer = 0;
	_ghostFearTimerMax = 3000;
	
	// initialise menu and pause states
	// pause
	_pause = new Menu;
	_pause->_backGround = new Texture2D;
	_pause->_rect = new Rect(0.0f, 0.0f, WWIDTH, WHEIGHT);
	_pause->_textRect = new Rect(
		WWIDTH / 4.0f,
		(WHEIGHT / 2.0f) + 50.0f,
		WWIDTH / 2.0f,
		100.0f);
	_pause->_keyDown = false;
	_pause->_stringPosition = new Vector2(WWIDTH / 2.3f,
		(WHEIGHT / 2.0f) + 100.0f);
	_pause->text = "_.-| P A U S E D |-._";

	_start = new Menu;
	_start->_backGround = new Texture2D;
	_start->_rect = new Rect(0.0f, 0.0f, WWIDTH, WHEIGHT);
	_start->_textRect = new Rect(
		WWIDTH / 4.0f,
		(WHEIGHT / 2.0f) + 50.0f,
		WWIDTH / 2.0f,
		100.0f);
	_start->_keyDown = false;
	_start->_stringPosition = new Vector2(WWIDTH/ 2.3f,
		(WHEIGHT/ 2.0f) + 100.0f);
	_start->text = "_. - | Press SPACE BAR to start | -._";
	_start->_playedSound = false;
	
	// music & soundeffects
	_pop = new SoundEffect();
	_intro = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, WWIDTH, WHEIGHT, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	DeletePlayer(_pacman);
	delete _pacman;

	for (int i = 0; i < NUM_OF_MUNCHIES; i++) {
		DeleteMunchie(_munchies[i]);
	}
	delete _munchies;
	delete _munchieTexture;
	
	for (int i = 0; i < NUM_OF_WALLS; i++) {
		DeleteObstacle(_walls[i]);
	}
	delete _walls;
	delete _wallTexture;

	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		DeleteEntity(&_ghosts[i]->self);
	}
	delete _ghosts;

	DeleteEntity(&_fruits->self);
	delete _fruits;

	DeleteEntity(&_powerUp->self);
	delete _powerUp;

	DeleteMenu(_pause);
	delete _pause;

	DeleteMenu(_start);
	delete _start;

	delete _pop;
}

void Pacman::DeleteEntity(Entity* obj)
{
	delete obj->position;
	delete obj->sourceRect;
	delete obj->texture;
}

void Pacman::DeletePlayer(Player* obj) {
	delete obj->texture;
	delete obj->position;
	delete obj->sourceRect;
	delete obj->direction;
}

void Pacman::DeleteSelf(Entity* obj) {
	delete obj->position;
	delete obj->sourceRect;
	delete obj->texture;
}

void Pacman::DeleteMunchie(Munchie* obj) {
	DeleteEntity(&obj->self);
}

void Pacman::DeleteMenu(Menu* obj) {
	delete obj->_backGround;
	delete obj->_rect;
	delete obj->_textRect;
	delete obj->_stringPosition;
}

void Pacman::DeleteObstacle(Obstacle* obj) {
	DeleteEntity(&obj->self);
}

void Pacman::LoadContent()
{
	// Load Map
	LoadMap();

	// Load Pacman
	_pacman->texture->Load("Textures/Pacman.tga", false);

	// Load Munchies
	_munchieTexture->Load("Textures/Munchie.png", true);
	for (int i = 0; i < NUM_OF_MUNCHIES; i++) {
		_munchies[i]->self.texture = _munchieTexture;
	}
	
	// Load Walls
	_wallTexture->Load("Textures/wallTile.tga", true);
	for (int i = 0; i < NUM_OF_WALLS; i++) {
		_walls[i]->self.texture = _wallTexture;
	}

	// Load Ghosts
	int ghostID = 0;
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		switch (ghostID) {
		case 0:
			_ghosts[i]->self.texture->Load("Textures/Ghosts_Red.png", true);
			break;
		case 1:
			_ghosts[i]->self.texture->Load("Textures/Ghosts_Pink.png", true);
			break;
		case 2:
			_ghosts[i]->self.texture->Load("Textures/Ghosts_Blue.png", true);
			break;
		case 3:
			_ghosts[i]->self.texture->Load("Textures/Ghosts_Orange.png", true);
		default:
			ghostID = 0;
			break;
		}
		
		if (ghostID < 3) {
			ghostID++;
		} else {
			ghostID = 0;
		}
	}

	// Load Fruits
	_fruitTexture->Load("Textures/Bonus.png", true);
	_fruits->self.texture = _fruitTexture;

	// Load Bonus
	_powerUp->self.texture->Load("Textures/Bonus.png", false);

	// Load Menu Images
	_pause->_backGround->Load("Textures/Transparency.png", false);
	_start->_backGround->Load("Textures/Transparency.png", false);
	
	/* TODO
	 * ====
	 * 
	 * - Load Pills/Pickups/Fruit/Power-Ups
	 */

	// Load Music & Sounds
	// eat a munchie
	_pop->Load("Sounds/pop.wav");

	// game intro
	_intro->Load("Sounds/pacman_beginning.wav");

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
	_stringDirection = new Vector2(10.0f, 50.0f);
	_stringScore = new Vector2(10.0f, 75.0f);
	
}

void Pacman::LoadMap() {
	string mapString = "level_0.txt";
	std::ifstream mapFile(mapString);

	string rawRow = "";
	int rowCount = 0;

	if (mapFile.is_open()) {
		while (getline(mapFile, rawRow, '\n')) {

			if (rowCount > MAP_ROWS) {
				cout << "Level too big - too many rows!!" << endl;
				mapFile.close();
				exit(-1);
			}

			for (int i = 0; i < MAP_COLS; i++) {
				
				if (rawRow.length() > MAP_COLS) {
					cout << "ROW Exceeds " << MAP_COLS <<
						" (" << rawRow.length() << ")" << endl;
				}
				if (rawRow.length() > i) {
					map[rowCount][i] = (char)rawRow.at(i);
				}
			}

			rowCount++;

			rawRow.clear();
		}
	}

	InitialiseMunchies();
	InitialiseWalls();
	InitialiseGhosts();
	DefineMap();
}

void Pacman::InitialiseMunchies() {
	for (int i = 0; i < NUM_OF_MUNCHIES; i++) {
		//all munchies
		_munchies[i] = new Munchie();
		_munchies[i]->self = Entity();
		_munchies[i]->self.sourceRect = new Rect();
		_munchies[i]->self.position = new Rect();
		_munchies[i]->self.texture = new Texture2D();
		_munchies[i]->self.texture = _munchieTexture;
		_munchies[i]->isEaten = false;
		_munchies[i]->isPowerPellet = false;
		_munchies[i]->value = 0;
		_munchies[i]->scale = 0;
		_munchies[i]->offset = 0;
	}
}

void Pacman::InitialiseWalls() {
	for (int i = 0; i < NUM_OF_WALLS; i++) {
		_walls[i] = new Obstacle();
		_walls[i]->self = Entity();
		_walls[i]->self.position = new Rect();
		_walls[i]->self.sourceRect = new Rect();
		_walls[i]->self.texture = new Texture2D();
		_walls[i]->self.texture = _wallTexture;
		_walls[i]->canBonusSpawn = false;
		_walls[i]->canGhostsPass = false;
		_walls[i]->canGhostsSpawn = false;
		_walls[i]->canPlayerPass = false;
		_walls[i]->canPlayerSpawn = false;
	}
}

void Pacman::InitialiseGhosts() {
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		//all ghosts
		_ghosts[i] = new Enemy();
		_ghosts[i]->self = Entity();
		_ghosts[i]->self.sourceRect = new Rect();
		_ghosts[i]->self.position = new Rect();
		_ghosts[i]->self.texture = new Texture2D();
		_ghosts[i]->currFrame = 0;
		_ghosts[i]->maxFrame = 2;
		_ghosts[i]->isAlive = true;
		_ghosts[i]->isChasing = true;
		_ghosts[i]->isEatable = false;
		_ghosts[i]->isSafe = true;
		_ghosts[i]->speed = 0.1f;
		_ghosts[i]->facing = Direction::dDOWN;
		_ghosts[i]->motion = Movement::mSTOP;
		_ghosts[i]->nextMove = Movement::mSTOP;
	}
}

void Pacman::DefineMap() {
	int _munchieCounter = 0;
	int _wallCounter = 0;
	int _ghostCounter = 0;
	Vector2i tmpTile = Vector2i();

	for (int row = 0; row < MAP_ROWS; row++) {
		for (int col = 0; col < MAP_COLS; col++) {
			switch (map[row][col]) {
			case 'P':
				//store start position
				_pacmanStartPos = Vector2i(col * TILE_SIZE, row * TILE_SIZE);

				//place pacman
				_pacman->position = new Vector2(_pacmanStartPos.get());
				/*
				_pacman->position = new Vector2(
					col * TILE_SIZE,
					row * TILE_SIZE
				);
				*/
				break;
			case '+':
			case '.':
				//all munchies
				_munchies[_munchieCounter]->self.sourceRect = new Rect(0.0f, 0.0f, MUNCHIE_SIZE, MUNCHIE_SIZE);
				_munchies[_munchieCounter]->self.position = new Rect(
					(col)*TILE_SIZE,
					(row)*TILE_SIZE,
					MUNCHIE_SIZE, MUNCHIE_SIZE
				);

				_munchies[_munchieCounter]->isEaten = false;

				//power munchies
				if (map[row][col] == '+') {
					_munchies[_munchieCounter]->isPowerPellet = true;
					_munchies[_munchieCounter]->value = 50;
					_munchies[_munchieCounter]->scale = 2.0f;
					_munchies[_munchieCounter]->offset = (TILE_SIZE / 2) / _munchies[_munchieCounter]->scale;

				} else {
					_munchies[_munchieCounter]->isPowerPellet = false;
					_munchies[_munchieCounter]->value = 10;
					_munchies[_munchieCounter]->scale = 1.0f;
					_munchies[_munchieCounter]->offset = TILE_SIZE / 2;
				}

				//center munchie
				_munchies[_munchieCounter]->self.position->X +=
					_munchies[_munchieCounter]->offset - _munchies[_munchieCounter]->self.position->Width / 2;
				_munchies[_munchieCounter]->self.position->Y +=
					_munchies[_munchieCounter]->offset - _munchies[_munchieCounter]->self.position->Height / 2;

				_munchieCounter++;
				break;
			case '#':
				_walls[_wallCounter]->self.sourceRect = new Rect(0.0f, 0.0f, TILE_SIZE, TILE_SIZE);
				_walls[_wallCounter]->self.position = new Rect(
					(col)*TILE_SIZE,
					(row)*TILE_SIZE,
					TILE_SIZE, TILE_SIZE
				);
				_walls[_wallCounter]->canGhostsPass = false;
				_walls[_wallCounter]->canGhostsSpawn = false;
				_walls[_wallCounter]->canPlayerPass = false;
				_walls[_wallCounter]->canPlayerSpawn = false;
				_walls[_wallCounter]->canBonusSpawn = false;

				_wallCounter++;
				break;
			case 'G':
				if (_ghostCounter < NUM_OF_GHOSTS) {
					_ghosts[_ghostCounter]->self.sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
					_ghosts[_ghostCounter]->self.position = new Rect(
						col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE
					);
					_ghosts[_ghostCounter]->isAlive = true;
					_ghosts[_ghostCounter]->isChasing = true;

					_ghostCounter++;
				}
				break;
			case 'B':
				_fruits->isShown = true;
				_fruits->self.position = new Rect(
					col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE
				);
				break;
			default:
				break;
			}
		}
	}

	//add additional ghosts, if any
	//eg, NUM_OF_GHOSTS > 4
	if (_ghostCounter < NUM_OF_GHOSTS) {
		for (int i = _ghostCounter; i < NUM_OF_GHOSTS; i++) {
			tmpTile = GetRandomTile('.');
			_ghosts[_ghostCounter]->self.sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
			_ghosts[_ghostCounter]->self.position = new Rect(
				tmpTile.X * TILE_SIZE,
				tmpTile.Y * TILE_SIZE,
				TILE_SIZE, TILE_SIZE
			);
			_ghosts[_ghostCounter]->isAlive = true;
			_ghosts[_ghostCounter]->isChasing = true;

			_ghostCounter++;
		}
	}

	//DEBUG
	std::cout << "munchie number = " << _munchieCounter << endl;
	std::cout << "_munchies[_munchieCounter]->rect;" <<
		"\n     X: " << _munchies[_munchieCounter - 1]->self.position->X <<
		"\n     Y: " << _munchies[_munchieCounter - 1]->self.position->Y <<
		"\nHeight: " << _munchies[_munchieCounter - 1]->self.position->Height <<
		"\n Width: " << _munchies[_munchieCounter - 1]->self.position->Width << endl << endl;
	std::cout << "wall number = " << _wallCounter << endl;
	std::cout << "_walls[_wallCounter]->rect;" <<
		"\n     X: " << _walls[_wallCounter - 1]->self.position->X <<
		"\n     Y: " << _walls[_wallCounter - 1]->self.position->Y <<
		"\nHeight: " << _walls[_wallCounter - 1]->self.position->Height <<
		"\n Width: " << _walls[_wallCounter - 1]->self.position->Width << endl;

}

void Pacman::Update(int elapsedTime) {
	/* TODO:
	 * =====
	 *
	 * - AI movement for enemies/ghosts
	 * - Timed spawn of Fruit/PickUp
	 * - Update of score? (on player collision may be better
	*/

	/* PACMAN UPDATE */
	//cout << "elapsedTime : " << elapsedTime << "ms" << std::endl;
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();
	CheckPaused(keyboardState, Input::Keys::P, Input::Keys::SPACE);

	if (!_start->_playedSound) {
		Audio::Play(_intro);
		_start->_playedSound = true;
	}

	if (!_pause->_menu || !_start->_menu) {
		Input(elapsedTime, keyboardState, mouseState);
		UpdatePacman(elapsedTime);
		UpdateMunchie(elapsedTime);
		UpdateGhost(elapsedTime);
		UpdateBonuses(elapsedTime);
		CheckViewportCollision();
		CheckPacmanCollision();
		for (int i = 0; i < NUM_OF_GHOSTS; i++) {
			CheckViewportCollision(
				_ghosts[i]->self.position->X,
				_ghosts[i]->self.position->Y,
				_ghosts[i]->self.sourceRect->Width,
				_ghosts[i]->self.sourceRect->Height
			);
		}
	}
}

void Pacman::Draw(int elapsedTime) {
	/* TODO:
	 * =====
	 * 
	 * - Draw Ghosts/Enemies
	 * - Re Draw existing Munchies
	 * - Draw PowerUps/Pickups
	 *
	 */

	// Allows us to easily create a string
	std::stringstream stream, streamDirection, streamScore;
	/*stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;
	/*
	streamDirection << "Direction X: " << _pacman->direction->X << " Y: " << _pacman->direction->Y//;
	streamDirection << "LastMovement: " << GetMovementString(_pacman->lastMove) <<
		<< "CurrentMovement: " << GetMovementString(_pacman->currMove) <<
		" NextMovement: " << GetMovementString(_pacman->nextMove);
	*/
	stream << "Fruit Status - Hidden = " << _fruits->isShown <<
		" Delay = " << _fruits->spawnDelay <<
		" CountDown: " << _fruits->spawnTimer;
	streamDirection << "Boost: " << _pacman->boostDuration << 
		" Fear Timer: " << _ghostFearTimer <<
		" Ghost[1]->motion = " << (int)_ghosts[0]->motion;
	streamScore << "Score : " << _pacman->score;

	SpriteBatch::BeginDraw(); // Starts Drawing

	// Draws Pacman
	if (_pacman->lives >= 0) {
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);
	}

	// Draws Munchies
	for (int i = 0; i < NUM_OF_MUNCHIES - 1; i++) {
		if (!_munchies[i]->isEaten) {
			SpriteBatch::Draw(
				_munchies[i]->self.texture,
				_munchies[i]->self.position,
				_munchies[i]->self.sourceRect,
				Vector2::Zero,
				_munchies[i]->scale,
				0.0f, Color::White, SpriteEffect::NONE);
		}
	}

	// Draw Walls
	for (int i = 0; i < NUM_OF_WALLS; i++) {
		SpriteBatch::Draw(
			_walls[i]->self.texture,
			_walls[i]->self.position,
			_walls[i]->self.sourceRect,
			Vector2::Zero,
			_wallScale,
			0.0f, Color::White, SpriteEffect::NONE);
	}

	// Draw Ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		// Ghosts are chasing
		if (_ghosts[i]->isChasing) {
			SpriteBatch::Draw(
				_ghosts[i]->self.texture,
				_ghosts[i]->self.position,
				_ghosts[i]->self.sourceRect,
				Vector2::Zero,
				1.0,
				0.0f, Color::White, SpriteEffect::NONE);
		// Ghosts are being chased
		} else {
			
			SpriteBatch::Draw(
				_ghosts[i]->self.texture,
				_ghosts[i]->self.position,
				&Rect(
					_ghosts[i]->self.sourceRect->X,
					_ghosts[i]->self.sourceRect->Height * 4,
					_ghosts[i]->self.sourceRect->Width,
					_ghosts[i]->self.sourceRect->Height
				),
				Vector2::Zero,
				1.0,
				0.0f, Color::White, SpriteEffect::NONE);

			if (!_ghosts[i]->isAlive) {
				SpriteBatch::Draw(
					_ghosts[i]->self.texture,
					_ghosts[i]->self.position,
					&Rect(
						_ghosts[i]->self.sourceRect->X,
						_ghosts[i]->self.sourceRect->Height * 5,
						_ghosts[i]->self.sourceRect->Width,
						_ghosts[i]->self.sourceRect->Height
					),
					Vector2::Zero,
					1.0,
					0.0f, Color::White, SpriteEffect::NONE);
			}
		}
	}

	// Draw Fruits
	if (_fruits->isShown) {
		SpriteBatch::Draw(
			_fruits->self.texture,
			_fruits->self.position,
			_fruits->self.sourceRect,
			Vector2::Zero, 1.0f, 0.0f,
			Color::White, SpriteEffect::NONE);
	}

	// Reset animation loop
	if (_frameCount >= 60) {
		_frameCount = 0;
	}
	
	if (_pause->_menu) {
		std::stringstream menuStream;

		// pause pacman animation
		_pacman->currentFrametime = 0;

		if (_start->_menu) {
			menuStream << "_.-| Press SPACE BAR to start |-._";
			_start->_stringPosition->X = WWIDTH / 2.5f;
		} else {
			menuStream << "_.-| PAUSED! |-._";
			_pause->_stringPosition->X = WWIDTH / 2.3f;
		}

		SpriteBatch::Draw(_pause->_backGround, _pause->_rect, nullptr);
		SpriteBatch::DrawRectangle(_pause->_textRect, Color::Red);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pause->_stringPosition, Color::Green);
	} else {
		// enable munchie animations
		_frameCount++;

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
		SpriteBatch::DrawString(streamDirection.str().c_str(), _stringDirection, Color::Green);
		SpriteBatch::DrawString(streamScore.str().c_str(), _stringScore, Color::Green);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, 
	Input::KeyboardState* state,
	Input::MouseState* mouseState){
	// Gets the current state of the keyboard
	if (!_pause->_menu) {
		//Set pacman's LAST movement
		_pacman->lastMove = _pacman->currMove;

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
			if (!_pacman->isBoosting) {
				_pacman->isBoosting = true;
				
			} else {
				if (_pacman->boostDuration < _pacman->boostDurationMax) {
					_pacman->boostDuration++;
				}
			}
		}
		
		// Gets the current state of the left mouse button
		if (mouseState->LeftButton == Input::ButtonState::PRESSED) {
			//DEBUG
			//cout << "next move is > " << static_cast<int>(_pacman->nextMove) << endl;
			Vector2i mTile;
			mTile.X = floor(mouseState->X / TILE_SIZE);
			mTile.Y = floor(mouseState->Y / TILE_SIZE);
			cout << "Mouse LeftClick at: X-" << mouseState->X << ", Y-" << mouseState->Y <<
				"\n(maptile @ map[" << mTile.Y << "][" << mTile.X << "] = " << 
				map[mTile.Y][mTile.X] << ")";

		}

		// Gets the current state of right mouse button
		if (mouseState->RightButton == Input::ButtonState::PRESSED) {
			//to be refactored into a function
			if (!_fruits->isShown) {
				SpawnFruit();
			}
		}
	}
}

void Pacman::SpawnFruit() {
	Vector2i fTile = GetRandomTile('.');
	_fruits->isShown = true;
	_fruits->self.position = new Rect(
		fTile.X * TILE_SIZE,
		fTile.Y * TILE_SIZE,
		TILE_SIZE, TILE_SIZE
	);

	//DEBUG
	//cout << "FRUIT AT " << _fruits->self.position->X << ", " <<
	//	_fruits->self.position->Y << endl;
}

void UpdateBonuses(int elapsedTime) {
	// Draw Bonuses & PowerUps
	/*if (_frameCount % 20 == 0) {
		_fruits->self.source
	}*/
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey){
	// Checks if ESCAPE key is pressed
	if (state->IsKeyDown(Input::Keys::ESCAPE)) {
		Graphics::Destroy();
	}

	// Checks is StartGame key is pressed (Start Screen)
	if ((state->IsKeyDown(startKey))) {
		if (_start->_menu && _pause->_menu) {
			_start->_menu = false;
			_pause->_menu = false;
			if ((_intro->GetState() == SoundEffectState::PLAYING)) {
				_intro->SetLooping(false);
			}

		}
	}

	// Checks if Pause key is pressed (Pause Screen)
	if (!_pause->_keyDown && state->IsKeyDown(pauseKey)) {
		_pause->_menu = !_pause->_menu;
		_pause->_keyDown = true;
	}

	// 
	if (state->IsKeyUp(pauseKey)) {
		_pause->_keyDown = false;
	}
}

void Pacman::CheckPacmanCollision() {
	//use _pacmanPosition + _pacmanSourceRect
	//Get player position and size
	Rect tmpRect = Rect(
		_pacman->position->X,
		_pacman->position->Y,
		_pacman->sourceRect->Width,
		_pacman->sourceRect->Height
	);

	//check collision with munchies
	int _munchieCounter = 0;

	for (int i = 0; i < NUM_OF_MUNCHIES; i++) {
		if (!_munchies[i]->isEaten) {
			//test munchie position
			if (HasTargetHitObject(&tmpRect, _munchies[i]->self.position, 0)) {
				//set flag
				_munchies[i]->isEaten = true;

				//update score
				_pacman->score += _munchies[i]->value;

				//play soundfx
				Audio::Play(_pop);

				//scare ghosts!
				if (_munchies[i]->isPowerPellet) {
					ScareGhosts();
				}
				
				//DEBUG
				//cout << "Eaten munchie at X:" << _munchies[i]->self.position->X <<
				//	" Y:" << _munchies[i]->self.position->Y << endl;
			}
		}
	}

	if (_fruits->isShown) {
		if (HasTargetHitObject(&tmpRect, _fruits->self.position)) {
			// hide fruit
			_fruits->isShown = false;

			//update score
			_pacman->score += _fruits->value * (_fruits->num + 1);
			/*
			Note:
			  since "num" starts at zero, need to increment to ensure
			  that the fruit value is multiplied by a non-zero number
			*/

			//play sound fx

			//select next fruit
			if (_fruits->num >= _fruits->maxNum) {
				_fruits->num = 0;
			} else {
				_fruits->num++;
			}

			//reset delay
			_fruits->spawnTimer = 0;
		}
	}

	if (NUM_OF_GHOSTS > 0) {
		for (int i = 0; i < NUM_OF_GHOSTS; i++) {
			if (HasTargetHitObject(&tmpRect, _ghosts[i]->self.position, 0, 'B')) {
				if (_ghosts[i]->isEatable) {
					//eat ghost
					//score
					_pacman->score += _ghosts[i]->value;

					//reset ghost
					Vector2 tmpGhostPos = Vector2();
					tmpGhostPos = GetRandomTile('G').get();
					_ghosts[i]->self.position->X = tmpGhostPos.X * TILE_SIZE;
					_ghosts[i]->self.position->Y = tmpGhostPos.Y * TILE_SIZE;
					_ghosts[i]->isAlive = true;
					_ghosts[i]->isChasing = true;
					_ghosts[i]->isEatable = false;
					_ghosts[i]->isSafe = true;
					_ghosts[i]->facing = Direction::dDOWN;
					_ghosts[i]->motion = Movement::mSTOP;
					_ghosts[i]->nextMove = Movement::mSTOP;

				} else {
					//stop movement
					_pacman->nextMove = Movement::mSTOP;
					_pacman->currMove = Movement::mSTOP;

					//death animation trigger

					//decreament lives
					_pacman->lives--;

					//place pacman
					_pacman->position = new Vector2(_pacmanStartPos.get());
				}
			}

		}
	}
}

bool Pacman::HasHitWall(Rect* target, bool isPlayer = true,
	float targetTolerance, float wallTolerance) {
	//checks for player or ghosts
	bool passThroughable;

	//add buffer to walls
	Rect wall;
	float slim = wallTolerance;	//reduction in wall boundary
	
	for (int i = 0; i < NUM_OF_WALLS; i++) {
		if (isPlayer) {
			passThroughable = _walls[i]->canPlayerPass;
		} else {
			passThroughable = _walls[i]->canGhostsPass;
		}
		if (!passThroughable) {
			wall = *_walls[i]->self.position;
			wall.Width -= slim;
			wall.Height -= slim;
			wall.X += slim / 2;
			wall.Y += slim / 2;
			if (HasTargetHitObject(target, &wall, targetTolerance, 'c')) {
				//a wall has been hit
				//DEBUG
				cout << "PacMan;" <<
					"\n     X: " << target->X << "(" << ceil(target->X) / TILE_SIZE << ")" <<
					"\n     Y: " << target->Y << "(" << ceil(target->Y) / TILE_SIZE << ")" <<
					"\nHeight: " << target->Height <<
					"\n Width: " << target->Width << endl << endl;

				cout << "Wall [" << i << "];" <<
					"\n     X: " << wall.X << "(" << wall.X / TILE_SIZE << ")" <<
					"\n     Y: " << wall.Y << "(" << wall.Y / TILE_SIZE << ")" <<
					"\nHeight: " << wall.Height <<
					"\n Width: " << wall.Width << endl;
				return true;
			}
		}
	}

	//no walls hit
	return false;
}

bool Pacman::HasHitWall(Vector2i origin, Movement moveTo) {
	Vector2i tmpTile = Vector2i();

	tmpTile.X = floor(origin.X) / TILE_SIZE;
	tmpTile.Y = floor(origin.Y) / TILE_SIZE;

	//if (tmpTile.X > MAP_COLS || tmpTile.Y > MAP_ROWS) {
	//	return true;
	//} else {

	switch (moveTo) {
	case Movement::mLEFT:
		tmpTile.X--;
		break;
	case Movement::mRIGHT:
		tmpTile.X++;
		break;
	case Movement::mUP:
		tmpTile.Y--;
		break;
	case Movement::mDOWN:
		tmpTile.Y++;
		break;
	case Movement::mSTOP:
	default:
		return true;
		break;
	}

	//TODO: check types of tiles that pacman can move on/through
	switch (map[tmpTile.Y][tmpTile.X]) {
	case '#':
		//DEBUG
		//cout << "Tile at[" << tmpTile.Y << "][" << tmpTile.X << "] is a wall!" << endl;
		return false;
		break;
	case '<':
	case '>':
		return true;
		break;
	case ' ':
	default:
		//DEBUG
		//cout << "Tile at[" << tmpTile.Y << "][" << tmpTile.X << "] is empty!" << endl;
		return true;
		break;
	}
}



std::string Pacman::GetMovementString(Movement movement) {
	switch (movement) {
	case Movement::mDOWN:
		return "mDOWN";
		break;
	case Movement::mLEFT:
		return "mLEFT";
		break;
	case Movement::mRIGHT:
		return "mRIGHT";
		break;
	case Movement::mSTOP:
		return "mSTOP";
		break;
	case Movement::mUP:
		return "mUP";
		break;
	default:
		return "ERROR - NOT DEFINED!";
		break;
	}
}

bool Pacman::HasTargetHitObject(Rect* git, Rect* obj, float tolerance, char mode) {
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
			//DEBUG
			cout << "SAUSAGES!!!" << endl;
			cout << "TARGET;" <<
				"\n     X: " << tmpTarget.X << "(" << ceil(tmpTarget.X) / TILE_SIZE << ")" <<
				"\n     Y: " << tmpTarget.Y << "(" << ceil(tmpTarget.Y) / TILE_SIZE << ")" <<
				"\nHeight: " << tmpTarget.Height <<
				"\n Width: " << tmpTarget.Width << endl << endl;

			cout << "OBJECT;" << 
				"\n     X: " << obj->X << "(" << obj->X / TILE_SIZE << ")" <<
				"\n     Y: " << obj->Y << "(" << obj->Y / TILE_SIZE << ")" <<
				"\nHeight: " << obj->Height <<
				"\n Width: " << obj->Width << endl;

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

			//DEBUG
			/*
			G-O horizontal line squared = gitCenterX squared - objCenterX squared
			G-O vertical line squared = gitCenterY squared - objCenterY squared
			G-O diagonal line squared = G-O horizontal line squared + G-O vertical line squared
			*/

			//pythag
			/*
			cout << "Pythag Collison:" <<
				"\nabs(gitCenterX - objCenterX) = " <<
				gitCenterX << " - " << objCenterX << " = " << abs(gitCenterX - objCenterX) <<
				"\nabs(gitCenterY - objCenterY) = " <<
				gitCenterY << " - " << objCenterY << " = " << abs(gitCenterY - objCenterY) << endl;
			cout << "Target Distance = " << dist <<
				" Collision Range = " << range << endl;
			*/
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

void Pacman::CheckViewportCollision(){
	/* wrap-around */
	if (_pacman->position->Y < 0.0f) {
		_pacman->position->Y = WHEIGHT - _pacman->sourceRect->Height;
	}

	if (_pacman->position->Y > WHEIGHT - _pacman->sourceRect->Height) {
		_pacman->position->Y = 1.0f;
	}

	if (_pacman->position->X < 0.0f) {
		_pacman->position->X = WWIDTH- _pacman->sourceRect->Width;
	}

	if (_pacman->position->X > WWIDTH - _pacman->sourceRect->Width) {
		_pacman->position->X = 1.0f;
	}

	CheckWarpCollision(_pacman->position->X, _pacman->position->Y);
}

void Pacman::CheckWarpCollision(float& posX, float& posY) {
	Vector2i tmpTile;
	tmpTile.X = floor(posX / TILE_SIZE);
	tmpTile.Y = floor(posY / TILE_SIZE);

	switch (IdentifyTile(tmpTile.Y, tmpTile.X)) {
	case '<':
		tmpTile = GetRandomTile('>');

		posX = (tmpTile.X - 1) * TILE_SIZE;
		posY = tmpTile.Y * TILE_SIZE;
		break;
	case '>':
		tmpTile = GetRandomTile('<');

		posX = (tmpTile.X + 1) * TILE_SIZE;
		posY = tmpTile.Y * TILE_SIZE;
		break;
	default:
		break;
	}
}

void Pacman::CheckViewportCollision(float& X, float& Y, int& Width, int& Height) {
	/* wrap-around */
	if (Y < 0.0f) {
		Y = WHEIGHT - Height;
	}

	if (Y > WHEIGHT - Height) {
		Y = 0.0f;
	}

	if (X < 0.0f) {
		X = WWIDTH - Width;
	}

	if (X > WWIDTH - Width) {
		X = 0.0f;
	}
}

void Pacman::UpdatePacman(int elapsedTime){
	Rect tmpRect = Rect();
	Vector2 tmpDir = Vector2();
	float tmpSpeed = _pacman->speed * elapsedTime;
	
	if (_pacman->isBoosting) {
		if (_pacman->boostDuration > 0 &&
			_pacman->boostDuration <= _pacman->boostDurationMax) {
			tmpSpeed = _pacman->speed * elapsedTime * _pacman->boostMultiply;

			_pacman->boostDuration--;
		}
		else {
			_pacman->isBoosting = false;
		}
	}

	//get player pos, rounded
	Vector2i tmpPos;
	tmpPos.X = floor(_pacman->position->X);
	tmpPos.Y = floor(_pacman->position->Y);

	//Update pacman animation
	_pacman->currentFrametime += elapsedTime;

	if (_pacman->currentFrametime > _cPacmanFrameTime) {
		_pacman->frame++;

		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentFrametime = 0;
	}

	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;

	//Get player position and size
	tmpRect = Rect(
		_pacman->position->X,
		_pacman->position->Y,
		_pacman->sourceRect->Width,
		_pacman->sourceRect->Height
	);

	/* NOTE:
		lock ghosts' movement to tile grid by only
		allowing direction changes when in line
		with map tile same a pacman.
	*/
	if ((tmpPos.X % TILE_SIZE < tmpSpeed) &&
		(tmpPos.Y % TILE_SIZE < tmpSpeed)) {
		
		// test next direction?
		if (_pacman->nextMove != Movement::mSTOP) {
			// next tile is a space
			if ((HasHitWall(tmpPos, _pacman->nextMove))) {
				if (!GetMapTile(tmpPos.Y, tmpPos.X, '=')) {
					// set new direction
					_pacman->currMove = _pacman->nextMove;
				}
			}
		}

		// if about to collide with a wall...
		if (_pacman->currMove != Movement::mSTOP) {

			// if not a space tile or if a wall tile
			if (!HasHitWall(tmpPos, _pacman->currMove)) {
				// re-align to tile grid
				_pacman->position->X = floor(tmpPos.X / TILE_SIZE) * TILE_SIZE;
				_pacman->position->Y = floor(tmpPos.Y / TILE_SIZE) * TILE_SIZE;

				// halt
				_pacman->currMove = Movement::mSTOP;

			}
			else {
			}
		}
	}

	// apply tested direction to pacman's direction
	tmpDir = ApplyMovement(_pacman->currMove, tmpSpeed);

	_pacman->direction->X = tmpDir.X;
	_pacman->direction->Y = tmpDir.Y;

	// apply facing direction to sprite
	if (_pacman->facing != IsFacing(_pacman->currMove) &&
		IsFacing(_pacman->currMove) != Direction::dNULL) {
		_pacman->facing = IsFacing(_pacman->currMove);
	}
	//_pacman->sourceRect->Y = (_pacman->sourceRect->Height * _pacman->facing)+3;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * (int)_pacman->facing;

	// apply direction to position
	_pacman->position->Y += _pacman->direction->Y;
	_pacman->position->X += _pacman->direction->X;

	// store last movement
	_pacman->lastMove = _pacman->currMove;
}

Direction Pacman::IsFacing(Movement movement) {
	switch (movement) {
	case Movement::mDOWN:
		return Direction::dDOWN;
		break;
	case Movement::mRIGHT:
		return Direction::dRIGHT;
		break;
	case Movement::mLEFT:
		return Direction::dLEFT;
		break;
	case Movement::mUP:
		return Direction::dUP;
		break;
	default:
		return Direction::dNULL;
		//return Direction::dRIGHT;
		break;
	}
}


//too ridgid
Movement Pacman::GetMapMovement(Vector2i tile) {
	switch (map[tile.Y][tile.X]) {
	case '^':
		return Movement::mUP;
		break;
	default:
		//return RandomMotion();
		break;
	}
}

bool Pacman::WillHitWall(Rect* target, Movement targetMove, float targetSpd, bool isPlayer) {
	Vector2 tmpDir = ApplyMovement(targetMove,targetSpd);

	target->X += tmpDir.X;
	target->Y += tmpDir.Y;

	if (HasHitWall(target, isPlayer, 2, 0)) {
		return true;
	}
	return false;
}

Vector2 Pacman::ApplyMovement(Movement direction, float velocity) {
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

void Pacman::UpdateBonuses(int elapsedTime) {
	// Select current/correct frame
	if (_frameCount % 30 == 0) {
		
		if (_fruits->curFrames < _fruits->maxFrames) {
			_fruits->curFrames++;
		} else {
			_fruits->curFrames = 0;
		}
	}

	// Select current fruit
	_fruits->self.sourceRect->X =
		_fruits->self.sourceRect->Width * _fruits->num;

	// Select version of fruit
	_fruits->self.sourceRect->Y =
		_fruits->self.sourceRect->Height * _fruits->curFrames;

	// Auto spawn timer
	if (_fruits->spawnTimer >= _fruits->spawnMaxTime) {
		_fruits->spawnTimer = 0;
	} else {
		if (_frameCount % 20 == 0) {
			_fruits->spawnTimer++;
		}
	}

	if (_fruits->spawnTimer >= _fruits->spawnDelay) {
		if (!_fruits->isShown) {
			SpawnFruit();
		} else {
			_fruits->isShown = false;
		}

		_fruits->spawnTimer = 0;
	}
}

void Pacman::UpdateGhost(int elapsedTime) {
	Vector2i tmpVect;
	float tmpSpeed;

	// Draw Ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		// Get Ghosts' speed
		tmpSpeed = _ghosts[i]->speed * elapsedTime;

		// Get Ghosts' normalized position
		tmpVect.X = floor(_ghosts[i]->self.position->X);
		tmpVect.Y = floor(_ghosts[i]->self.position->Y);

		// Animate Ghosts
		if (_frameCount % 15 == 0) {
			if (_ghosts[i]->currFrame < _ghosts[i]->maxFrame) {
				_ghosts[i]->currFrame++;
			}
			else {
				_ghosts[i]->currFrame = 0;
			}
		}

		// Set Animation Frame
		_ghosts[i]->self.sourceRect->X =
			_ghosts[i]->self.sourceRect->Width * _ghosts[i]->currFrame;

		// Set Facing Direction
		_ghosts[i]->self.sourceRect->Y =
			_ghosts[i]->self.sourceRect->Height * (int)_ghosts[i]->facing;

		/* NOTE:
			lock ghosts' movement to tile grid by only
			allowing direction changes when in line
			with map tile same a pacman.
			( see UpdatePacman function)
		*/
		if ((tmpVect.X % TILE_SIZE < tmpSpeed) &&
			(tmpVect.Y % TILE_SIZE < tmpSpeed)) {
			//test direction
			if (_ghosts[i]->motion != Movement::mSTOP) {
				// Check map collisions
				if (!HasHitWall(tmpVect, _ghosts[i]->motion)) {
					_ghosts[i]->motion = NotSoRandomMotion(tmpVect);
				} else {
					CheckWarpCollision(_ghosts[i]->self.position->X, _ghosts[i]->self.position->Y);
				}
			} else {
				// if stopped
				// align to tile grid
				_ghosts[i]->self.position->X = floor(tmpVect.X / TILE_SIZE) * TILE_SIZE;
				_ghosts[i]->self.position->Y = floor(tmpVect.Y / TILE_SIZE) * TILE_SIZE;

				// choose a direction
				_ghosts[i]->motion = NotSoRandomMotion(tmpVect);
			}

			// Move ghosts out of base
			if (GetMapTile(
				floor(tmpVect.Y / TILE_SIZE),
				floor(tmpVect.X / TILE_SIZE),
				'^')) {
				_ghosts[i]->motion = Movement::mUP;
			}
		} else {
			//DEBUG
			// keep ghost off of walls due to frame skips
			if (GetMapTile(
				floor(tmpVect.Y / TILE_SIZE),
				floor(tmpVect.X / TILE_SIZE),
				'#')) {
				// Move Ghosts
				switch (_ghosts[i]->motion) {
				case Movement::mDOWN:
					_ghosts[i]->self.position->Y -= TILE_SIZE;
					break;
				case Movement::mUP:
					_ghosts[i]->self.position->Y += TILE_SIZE;
					break;
				case Movement::mLEFT:
					_ghosts[i]->self.position->X += TILE_SIZE;
					break;
				case Movement::mRIGHT:
					_ghosts[i]->self.position->X -= TILE_SIZE;
					break;
				default:
					/*
					_ghosts[i]->self.position->X = floor(tmpVect.X / TILE_SIZE) * TILE_SIZE;
					_ghosts[i]->self.position->Y = floor(tmpVect.Y / TILE_SIZE) * TILE_SIZE;
					*/
					break;
				}

				// stop ghost from moving further off track
				_ghosts[i]->motion = Movement::mSTOP;
			}
		}

		// Move Ghosts
		switch (_ghosts[i]->motion) {
		case Movement::mDOWN:
			_ghosts[i]->self.position->Y += tmpSpeed;
			break;
		case Movement::mUP:
			_ghosts[i]->self.position->Y -= tmpSpeed;
			break;
		case Movement::mLEFT:
			_ghosts[i]->self.position->X -= tmpSpeed;
			break;
		case Movement::mRIGHT:
			_ghosts[i]->self.position->X += tmpSpeed;
			break;
		default:
			_ghosts[i]->self.position->X = floor(tmpVect.X / TILE_SIZE) * TILE_SIZE;
			_ghosts[i]->self.position->Y = floor(tmpVect.Y / TILE_SIZE) * TILE_SIZE;
			break;
		}

		if (_ghosts[i]->isAlive && _ghosts[i]->isSafe) {
			switch (map	[tmpVect.Y / TILE_SIZE][tmpVect.X / TILE_SIZE]) {
			case '^':
				break;
			case '-':
			case 'G':
				_ghosts[i]->isSafe = true;
				break;
			case '=':
				_ghosts[i]->isSafe = false;
				break;
			default:
				break;
			}
		}

		// Set Facing direction to match Motion direction
		if (_ghosts[i]->motion != Movement::mSTOP) {
			_ghosts[i]->facing = IsFacing(_ghosts[i]->motion);
		}

		// Manage Fear/Chasing States
		if (_ghostFearTimer > 0) {
			_ghostFearTimer--;
		}
		else {
			_ghosts[i]->isChasing = true;
			_ghosts[i]->isEatable = false;
		}
	}
}

Movement Pacman::RandomMotion() {
	switch (rand() % 4) {
	case 0:
		return Movement::mDOWN;
		break;
	case 1:
		return Movement::mLEFT;
		break;
	case 2:
		return Movement::mRIGHT;
		break;
	case 3:
		return Movement::mUP;
		break;
	}
}

Movement Pacman::NotSoRandomMotion(Vector2i pos) {
	Movement result = Movement::mSTOP;
	Movement moves[4] = {						// array of movement choices available
		Movement::mSTOP, Movement::mSTOP,
		Movement::mSTOP, Movement::mSTOP
	};

	int checkMoves = 0;							// counter to check number of moves available
	int row = floor(pos.Y / TILE_SIZE);
	int col = floor(pos.X / TILE_SIZE);

	// fill array with available movements
	moves[0] = GetMapTile(row, col - 1, '#') ? Movement::mSTOP : Movement::mLEFT;
	moves[1] = GetMapTile(row, col + 1, '#') ? Movement::mSTOP : Movement::mRIGHT;
	moves[2] = GetMapTile(row - 1, col, '#') ? Movement::mSTOP : Movement::mUP;
	moves[3] = GetMapTile(row + 1, col, '#') ? Movement::mSTOP : Movement::mDOWN;

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


/* this would be a duplicate of GetRandomTile() function
Vector2i Pacman::FindTile(char target) {

}*/

bool Pacman::GetMapTile(int row, int col, char tile) {
	return map[row][col] == tile ? true : false;
}

char Pacman::IdentifyTile(int row, int col) {
	return map[row][col];
}

Vector2i Pacman::GetRandomTile(char tile) {
	int counter = 0;
	vector<Vector2i> tmpArray(0);
	Vector2i tmpVect = Vector2i();

	for (int row = 0; row < MAP_ROWS; row++) {
		for (int col = 0; col < MAP_COLS; col++) {
			if (map[row][col] == tile) {
				tmpVect.X = col;
				tmpVect.Y = row;

				tmpArray.push_back(tmpVect);

				counter++;
			}
		}
	}

	return tmpArray[rand() % counter];

	//return Vector2i();
}

void Pacman::ScareGhosts() {
	if (_ghostFearTimer < 1) {
		_ghostFearTimer = _ghostFearTimerMax;

		for (int i = 0; i < NUM_OF_GHOSTS; i++) {
			_ghosts[i]->isChasing = false;
			_ghosts[i]->isEatable = true;
		}
	}
}

void Pacman::UpdateMunchie(int elapsedTime){

	// Draw Munchies (big and small)
	for (int i = 0; i < NUM_OF_MUNCHIES; i++) {
		// Animate munchie
		if (_frameCount < 30) {
			// Select Red Munchie
			_munchies[i]->self.sourceRect->X = _munchies[i]->self.sourceRect->Width;
		} else {
			// Select Blue Munchie
			_munchies[i]->self.sourceRect->X = 0.0f;
		}
	}
}

/*
because S2D doesn't allow the use of custom fonts because fonts hard-coded
create a function/method to handle the font I want

How?
- load the texture
- allocate characters to each glyph in font spite sheet
- draw composite sprites to location

*/
/*
void Pacman::DrawString(const char* text, int len, Vector2i pos) {
	Texture2D font = Texture2D();
	font.Load("Textures/DeLarge.tga",false);
	Rect charPos, glyphPos, stringPos;



	for (int i = 0; i < len; i++) {
		switch (toupper(text[i])) {
		case 'A':
			break;
		case 'C':
			break;
		case 'P':
			break;
		case 'M':
			break;
		case 'N':
			break;
		default:
			break;
		}
	}

}
*/
/*
void SpriteBatch::DrawString(const char* text, const Vector2* position, const Color* color)
{
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glColor4f(color->R, color->G, color->B, color->A);
	glRasterPos2f(position->X, position->Y);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)text);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}
*/