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

	// initialise pacman/player
	_pacman = new Player();
	_pacman->currentFrametime = 0;
	_pacman->frame = 0;
	_pacman->texture = new Texture2D();
	_pacman->position = new Vector2(350.0f, 350.0f);
	//_pacman->sourceRect = new Rect(3.0f, 3.0f, 27, 27);
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
	//Obstacle* _walls[NUM_OF_WALLS] = { nullptr };

	// initialise munchies
	_munchieTexture = new Texture2D();
	//Munchie* _munchies[NUM_OF_MUNCHIES] = { nullptr };

	// initialise ghosts
	_ghostTexture = new Texture2D();
	_ghostFearTimer = 0;
	_ghostFearTimerMax = 1500;
	
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
	
	// music & soundeffects
	_pop = new SoundEffect();

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

	DeleteEntity(&_cherry->self);
	delete _cherry;

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
	//DEBUG
	_ghostTexture->Load("Textures/Ghosts_Red.png", true);
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
		//_ghosts[i]->self.texture = _ghostTexture;
		if (ghostID < 3) {
			ghostID++;
		} else {
			ghostID = 0;
		}
	}

	// Load Menu Images
	_pause->_backGround->Load("Textures/Transparency.png", false);
	_start->_backGround->Load("Textures/Transparency.png", false);
	
	/* TODO
	 * ====
	 * 
	 * - Construct Level:
	 * load level_0.txt
	 * store level data in 2d array
	 * loop through each line
	 * place graphic depending on char in line
	 *
	 * - Load Ghosts/Enemies
	 * - Load Pills/Pickups/Fruit/Power-Ups
	 */

	// Load Music & Sounds
	_pop->Load("Sounds/pop.wav");

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

	//DEBUG
	string tmpArray[MAP_ROWS];

	if (mapFile.is_open()) {
		while (getline(mapFile, rawRow, '\n')) {

			if (rowCount > MAP_ROWS) {
				cout << "Level too big - too many rows!!" << endl;
				mapFile.close();
				exit(-1);
			}

			//DEBUG
			tmpArray[rowCount] = rawRow;

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
		//_ghosts[i]->self.texture = _ghostTexture;
		_ghosts[i]->currFrame = 0;
		_ghosts[i]->maxFrame = 2;
		_ghosts[i]->isAlive = true;
		_ghosts[i]->isChasing = true;
		_ghosts[i]->isSafe = true;
		_ghosts[i]->speed = 0.1f;
		_ghosts[i]->facing = Direction::dDOWN;
		_ghosts[i]->motion = Movement::mDOWN;
	}
}

void Pacman::DefineMap() {
	int _munchieCounter = 0;
	int _wallCounter = 0;
	int _ghostCounter = 0;

	for (int row = 0; row < MAP_ROWS; row++) {
		for (int col = 0; col < MAP_COLS; col++) {
			switch (map[row][col]) {
			case 'P':
				//place pacman
				_pacman->position = new Vector2(
					col * TILE_SIZE,
					row * TILE_SIZE
				);
				break;
			case '+':
			case '.':
				//all munchies
				_munchies[_munchieCounter]->self.sourceRect = new Rect(0.0f, 0.0f, 11, 11);
				_munchies[_munchieCounter]->self.position = new Rect(
					(col)*TILE_SIZE,
					(row)*TILE_SIZE,
					12, 12
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
				_ghosts[_ghostCounter]->self.sourceRect = new Rect(0, 0, TILE_SIZE, TILE_SIZE);
				_ghosts[_ghostCounter]->self.position = new Rect(
					col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE
				);
				_ghosts[_ghostCounter]->isAlive = true;
				_ghosts[_ghostCounter]->isChasing = true;

				_ghostCounter++;
				break;
			default:
				break;
			}
		}
	}

	//DEBUG
	cout << "munchie number = " << _munchieCounter << endl;
	cout << "_munchies[_munchieCounter]->rect;" <<
		"\n     X: " << _munchies[_munchieCounter - 1]->self.position->X <<
		"\n     Y: " << _munchies[_munchieCounter - 1]->self.position->Y <<
		"\nHeight: " << _munchies[_munchieCounter - 1]->self.position->Height <<
		"\n Width: " << _munchies[_munchieCounter - 1]->self.position->Width << endl << endl;
	cout << "wall number = " << _wallCounter << endl;
	cout << "_walls[_wallCounter]->rect;" <<
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
	CheckPaused(keyboardState, Input::Keys::P, Input::Keys::SPACE);

	if (!_pause->_menu || !_start->_menu) {
		Input(elapsedTime, keyboardState);
		UpdatePacman(elapsedTime);
		UpdateMunchie(elapsedTime);
		UpdateGhost(elapsedTime);
		CheckViewportCollision();
		CheckPacmanCollision();
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
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;
	//streamDirection << "Direction X: " << _pacman->direction->X << " Y: " << _pacman->direction->Y;
	/*streamDirection << "LastMovement: " << GetMovementString(_pacman->lastMove) <<
		"CurrentMovement: " << GetMovementString(_pacman->currMove) <<
		" NextMovement: " << GetMovementString(_pacman->nextMove);*/
	streamDirection << "Boost: " << _pacman->boostDuration << " Fear Timer: " << _ghostFearTimer;
	streamScore << "Score : " << _pacman->score;

	SpriteBatch::BeginDraw(); // Starts Drawing

	// Draws Pacman
	SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); 

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

			//DEBUG
			/*if (_munchies[i]->isPowerPellet) {
				cout << "PowerPellet [" << _munchies[i]->self.position->X << "][" <<
					_munchies[i]->self.position->Y << "] found!" << endl;
			}*/
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

	// Reset animation loop
	if (_frameCount >= 60) {
		_frameCount = 0;
	}
	
	if (_pause->_menu) {
		std::stringstream menuStream;
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
	}
	else {
		_frameCount++;

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
		SpriteBatch::DrawString(streamDirection.str().c_str(), _stringDirection, Color::Green);
		SpriteBatch::DrawString(streamScore.str().c_str(), _stringScore, Color::Green);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state){
	// Gets the current state of the keyboard
	if (!_pause->_menu) {
		//Set pacman's LAST movement
		_pacman->lastMove = _pacman->currMove;

		// Checks if W key is pressed
		if (state->IsKeyDown(Input::Keys::W)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mUP;
		}

		// Checks if S key is pressed
		else if (state->IsKeyDown(Input::Keys::S)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mDOWN;
		}

		// Checks if A key is pressed
		else if (state->IsKeyDown(Input::Keys::A)) {
			//Set pacman's NEXT movement
			_pacman->nextMove = Movement::mLEFT;
		}

		// Checks if D key is pressed
		else if (state->IsKeyDown(Input::Keys::D)) {
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
		//DEBUG
		cout << "next move is > " << static_cast<int>(_pacman->nextMove) << endl;
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey){
	// Checks if ESCAPE key is pressed
	if (state->IsKeyDown(Input::Keys::ESCAPE)) {
		Graphics::Destroy();
	}

	// Checks is StartGame key is pressed (Start Screen)
	if (state->IsKeyDown(startKey)) {
		if (_start->_menu && _pause->_menu) {
			_start->_menu = false;
			_pause->_menu = false;
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
				cout << "Eaten munchie at X:" << _munchies[i]->self.position->X <<
					" Y:" << _munchies[i]->self.position->Y << endl;
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
		abs(gitCenterX - objCenterX) + 
		abs(gitCenterY - objCenterY)
	);
	float range = sqrt(((gitRadiusX + objRadiusX) + (gitRadiusY + objRadiusY)) / 2);

	//box collision adjustment
	Rect tmpTarget = Rect(
		git->X + (tolerance / 2),
		git->Y + (tolerance / 2),
		git->Width - tolerance,
		git->Height - tolerance
	);

	switch (mode) {
	case 'b':
	case 'B':
		//test for non-collisions
		/* old check 
			//is left1 and right1 further left than left2
		if ((tmpTarget.X < obj->X) &&
			(tmpTarget.X + tmpTarget.Width < obj->X)) {
			return false;
		} else if (
			//is left1 and right1 further right than right2
			(tmpTarget.X > obj->X + obj->Width) &&
			(tmpTarget.X + tmpTarget.Width > obj->X + obj->Width)) {
			return false;
		} else if (
			//is top1 and bot1 are higher than top2
			(tmpTarget.Y < obj->Y) &&
			(tmpTarget.Y + tmpTarget.Height < obj->Y)) {
			return false;
		} else if (
			//is top1 and bot1 are lower than bot2
			(tmpTarget.Y > obj->Y + obj->Height) &&
			(tmpTarget.Y + tmpTarget.Height > obj->Y + obj->Height)) { 
			return false;
			*/
		//if ((btm1 >= top2) && (top1 <= btm2) && (left1 <= right2) && (right1 >= left2))
		if (git->Y + git->Height >= obj->Y) {
			//bot1 > top2
			return false;
		} else if (git->Y <= obj->Y + obj->Height) {
			//top1 < bot2
			return false;
		} else if (git->X >= obj->X + obj->Width) {
			//left1 > right2
			return false;
		} else if (git->X + git->Width <= obj->X) {
			//right1 < left2
			return false;
		} else {
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
		}
		break;
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
			cout << "Pythag Collison:" <<
				"\nabs(gitCenterX - objCenterX) = " <<
				gitCenterX << " - " << objCenterX << " = " << abs(gitCenterX - objCenterX) <<
				"\nabs(gitCenterY - objCenterY) = " <<
				gitCenterY << " - " << objCenterY << " = " << abs(gitCenterY - objCenterY) << endl;
			cout << "Target Distance = " << dist <<
				" Collision Range = " << range << endl;
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
}

void Pacman::UpdatePacman(int elapsedTime){
	bool nextMoveValid = false;
	bool currMoveValid = false;
	bool canTurn = false;
	Rect tmpRect = Rect();
	Vector2 tmpDir = Vector2();
	Vector2i tmpTile = Vector2i();
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

	//get player pos
	Vector2i tmpPos;
	tmpPos.X = (int)ceil(_pacman->position->X);
	tmpPos.Y = (int)ceil(_pacman->position->Y);

	if (tmpPos.X % TILE_SIZE == 0 && tmpPos.Y % TILE_SIZE == 0) {
		canTurn = true;
	} else {
		canTurn = false;
	}

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

	tmpTile.X = ceil(_pacman->position->X);//% TILE_SIZE;
	tmpTile.Y = ceil(_pacman->position->Y);// / TILE_SIZE;
		
	// lock pacman's movement to tile grid
	// by only allowing direction changes when in line with tiles
	if ((tmpTile.X % TILE_SIZE < tmpSpeed) &&
		(tmpTile.Y % TILE_SIZE < tmpSpeed)) {
		// test next direction?
		if (//(_pacman->nextMove != Movement::mSTOP) &&
			(IsSpaceTile(tmpTile, _pacman->nextMove))) {

			// set new direction
			_pacman->currMove = _pacman->nextMove;
		} else {
		}
	}

	if ((_pacman->currMove != Movement::mSTOP) &&
		(!IsSpaceTile(tmpTile, _pacman->currMove))) {
		// halt
		_pacman->currMove = Movement::mSTOP;

		// re-align to tile grid
		_pacman->position->X = ceil(tmpTile.X / TILE_SIZE) * TILE_SIZE;
		_pacman->position->Y = ceil(tmpTile.Y / TILE_SIZE) * TILE_SIZE;
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

bool Pacman::IsSpaceTile(Vector2i origin, Movement moveTo) {
	Vector2i tmpTile = Vector2i();
	
	tmpTile.X = origin.X / TILE_SIZE;
	tmpTile.Y = origin.Y / TILE_SIZE;
	
	//if (tmpTile.X > MAP_COLS || tmpTile.Y > MAP_ROWS) {
	//	return true;
	//} else {

		switch (moveTo){
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
			cout << "Tile at[" << tmpTile.Y << "][" << tmpTile.X << "] is a wall!" << endl;
			return false;
			break;
		case '<':
		case '>':
			return true;
			break;
		case ' ':
		default:
			//DEBUG
			cout << "Tile at[" << tmpTile.Y << "][" << tmpTile.X << "] is empty!" << endl;
			return true;
			break;
		}
	//}
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

bool Pacman::WillHitWall(Rect* target, Movement targetMove, float targetSpd) {
	Vector2 tmpDir = ApplyMovement(targetMove,targetSpd);

	target->X += tmpDir.X;
	target->Y += tmpDir.Y;

	if (HasHitWall(target, true, 2, 0)) {
		return true;
	}
	return false;
}

Vector2 Pacman::ApplyMovement(Movement direction, float velocity) {
	Vector2 tmpDir = Vector2();

	switch (direction) {
	case Movement::mDOWN:
		//Moves Pacman Down Y axis
		tmpDir.Y = velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mUP:
		//Moves Pacman up Y axis
		tmpDir.Y = -velocity;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mLEFT:
		//Moves Pacman Left X axis
		tmpDir.X = -velocity;

		//Resets X axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mRIGHT:
		//Moves Pacman Right X axis
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

void Pacman::UpdateGhost(int elapsedTime) {
	// Draw Ghosts
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
		// Animate Ghosts
		if (_frameCount % 15 == 0) {
			if (_ghosts[i]->currFrame < _ghosts[i]->maxFrame) {
				_ghosts[i]->currFrame++;
			}
			else {
				_ghosts[i]->currFrame = 0;
			}
		}

		_ghosts[i]->self.sourceRect->X =
			_ghosts[i]->self.sourceRect->Width * _ghosts[i]->currFrame;

		_ghosts[i]->self.sourceRect->Y =
			_ghosts[i]->self.sourceRect->Height * (int)_ghosts[i]->facing;
		
		//DEBUG
		cout << "Ghosts sourceRect X= " << _ghosts[i]->self.sourceRect->X <<
			" Width= " << _ghosts[i]->self.sourceRect->Width << endl;
		
		// Move Ghosts
		switch (_ghosts[i]->motion) {
		case Movement::mDOWN:
			_ghosts[i]->self.position->Y += _ghosts[i]->speed * elapsedTime;
			break;
		case Movement::mUP:
			_ghosts[i]->self.position->Y -= _ghosts[i]->speed * elapsedTime;
			break;
		case Movement::mLEFT:
			_ghosts[i]->self.position->X -= _ghosts[i]->speed * elapsedTime;
			break;
		case Movement::mRIGHT:
			_ghosts[i]->self.position->X += _ghosts[i]->speed * elapsedTime;
			break;
		default:
			_ghosts[i]->motion = RandomMotion();
			break;
		}

		Vector2i tmpVect;
		/*if (_ghosts[i]->motion == Movement::mLEFT) {
			tmpVect.X = _ghosts[i]->self.position->X - _ghosts[i]->self.sourceRect->Width;
		} else {*/
			tmpVect.X = _ghosts[i]->self.position->X - (_ghosts[i]->self.sourceRect->Width);
		//}

		/*if (_ghosts[i]->motion == Movement::mUP) {
			tmpVect.X = _ghosts[i]->self.position->Y - _ghosts[i]->self.sourceRect->Height;
		} else {*/
			tmpVect.Y = _ghosts[i]->self.position->Y - (_ghosts[i]->self.sourceRect->Height);
		//}
		
		// Check collisions
		if (!IsSpaceTile(tmpVect, _ghosts[i]->motion)) {
			_ghosts[i]->motion = Movement::mSTOP;
		}

		// Move ghosts out of base
		if (_ghosts[i]->isAlive && _ghosts[i]->isSafe) {
			switch (map[tmpVect.Y / TILE_SIZE][tmpVect.X / TILE_SIZE]) {
			case '^':
				_ghosts[i]->motion = Movement::mUP;
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

		_ghosts[i]->facing = IsFacing(_ghosts[i]->motion);

		if (_ghostFearTimer > 0) {
			_ghostFearTimer--;
		} else {
			_ghosts[i]->isChasing = true;
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

void Pacman::ScareGhosts() {
	if (_ghostFearTimer < 1) {
		_ghostFearTimer = _ghostFearTimerMax;

		for (int i = 0; i < NUM_OF_GHOSTS; i++) {
			_ghosts[i]->isChasing = false;
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