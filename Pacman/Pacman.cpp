#include "Pacman.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

Pacman::Pacman(int argc, char* argv[]) : 
	Game(argc, argv), _cPacmanSpeed(0.2f), _cPacmanFrameTime(250)
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
	_pacman->score = 0;

	// initialise walls
	_wallTexture = new Texture2D();
	//_wallCounter = 0;
	_wallScale = 1.0f;
	Obstacle* _walls[NUM_OF_WALLS] = { nullptr };

	// initialise munchies
	_munchieTexture = new Texture2D();
	//_munchieCounter = 0;
	Munchie* _munchies[NUM_OF_MUNCHIES] = { nullptr };
	
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
	//_mainMenu = true;

	//Initialise important Game aspects
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

	DeleteEntity(&_cherry->self);
	delete _cherry;

	DeleteEntity(&_powerUp->self);
	delete _powerUp;

	DeleteMenu(_pause);
	delete _pause;

	DeleteMenu(_start);
	delete _start;
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
		_walls[i]->canBonusSpawn = false;
		_walls[i]->canGhostsPass = false;
		_walls[i]->canGhostsSpawn = false;
		_walls[i]->canPlayerPass = false;
		_walls[i]->canPlayerSpawn = false;
	}
}

void Pacman::DefineMap() {
	int _munchieCounter = 0;
	int _wallCounter = 0;

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
				_munchies[_munchieCounter] = new Munchie();
				_munchies[_munchieCounter]->self = Entity();
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
				_walls[_wallCounter] = new Obstacle();
				_walls[_wallCounter]->self = Entity();
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
	streamDirection << "LastMovement: " << GetMovementString(_pacman->lastMove) <<
		"CurrentMovement: " << GetMovementString(_pacman->currMove) <<
		" NextMovement: " << GetMovementString(_pacman->nextMove);
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

		//DEBUG
		cout << "next move is > " << _pacman->nextMove;
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

	for (int i = 0; i < _munchieCounter; i++) {
		if (!_munchies[i]->isEaten) {
			//test munchie position
			if (HasTargetHitObject(&tmpRect, _munchies[i]->self.position, 0)) {
				//set flag
				_munchies[i]->isEaten = true;

				//update score
				_pacman->score += _munchies[i]->value;

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

	float dist = (
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
			return false;
		} else if (git->Y <= obj->Y + obj->Height) {
			return false;
		} else if (git->X <= obj->X + obj->Width) {
			return false;
		} else if (git->X + git->Width + obj->X) {
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

	if (_pacman->position->X < 1.0f) {
		_pacman->position->X = WHEIGHT - _pacman->sourceRect->Width;
	}

	if (_pacman->position->X > WHEIGHT - _pacman->sourceRect->Width) {
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

	tmpTile.X = ceil(_pacman->position->X) / TILE_SIZE;
	tmpTile.Y = ceil(_pacman->position->Y) / TILE_SIZE;
		
	bool isWalkable = IsSpaceTile(tmpTile, _pacman->nextMove);

	//test pacman's next move
	if (isWalkable &&
		_pacman->nextMove != Movement::mSTOP) {
		//DEBUG
		cout << "nextMove is valid" << endl;

		nextMoveValid = true;

		if (canTurn) {
			//DEBUG
			cout << "setting next move as current move" << endl;

			_pacman->currMove = _pacman->nextMove;
		}
	} else {
		//DEBUG
		cout << "nextMove is NOT valid" << endl;

		nextMoveValid = false;
	}

	/*
	if (_pacman->nextMove != Movement::mSTOP //&& _pacman->nextMove != _pacman->currMove
		) {
		if (!WillHitWall(&tmpRect, _pacman->nextMove, tmpSpeed)) {
			//DEBUG
			cout << "nextMove is valid" << endl;

			nextMoveValid = true;

			if (canTurn) {
				//DEBUG
				cout << "setting next move as current move" << endl;

				_pacman->currMove = _pacman->nextMove;
			}

		} else {
			//DEBUG
			cout << "nextMove is NOT valid" << endl;

			nextMoveValid = false;
		}
	}
	*/
	
	//test pacman's current move
	if (_pacman->currMove != Movement::mSTOP) {
		if (!WillHitWall(&tmpRect, _pacman->currMove, tmpSpeed)) {
			//DEBUG
			cout << "curr move is valid" << endl;

			currMoveValid = true;

		} else {
			//DEBUG
			cout << "curr move is NOT valid" << endl;

			currMoveValid = false;

			_pacman->currMove = Movement::mSTOP;

			//center pacman to current tile
			_pacman->position->X = tmpTile.X * TILE_SIZE;
			_pacman->position->Y = tmpTile.Y * TILE_SIZE;
		}
	} else {
		//DEBUG
		cout << "curr move is STOP" << endl;


		if (nextMoveValid && canTurn) {
			//DEBUG
			cout << "doing next move" << endl;

			_pacman->currMove = _pacman->nextMove;
		}
	}

	// apply tested direction to pacman's direction
	tmpDir = ApplyMovement(_pacman->currMove, tmpSpeed);

	_pacman->direction->X = tmpDir.X;
	_pacman->direction->Y = tmpDir.Y;

	// apply facing direction to sprite
	if (_pacman->facing != IsFacing(_pacman->currMove) &&
		IsFacing(_pacman->currMove) != dNULL) {
		_pacman->facing = IsFacing(_pacman->currMove);
	}
	//_pacman->sourceRect->Y = (_pacman->sourceRect->Height * _pacman->facing)+3;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->facing;

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
		return dNULL;
		//return Direction::dRIGHT;
		break;
	}
}

bool Pacman::IsSpaceTile(Vector2i origin, Movement moveTo) {
	Vector2i tmpTile = Vector2i();
	
	if (origin.X > MAP_COLS || origin.Y > MAP_ROWS) {
		return true;
	} else {
		tmpTile.X += origin.X;
		tmpTile.Y += origin.Y;

		switch (moveTo){
		case mLEFT:
			tmpTile.X--;
			break;
		case mRIGHT:
			tmpTile.X++;
			break;
		case mUP:
			tmpTile.Y--;
			break;
		case mDOWN:
			tmpTile.Y++;
			break;
		case mSTOP:
		default:
			return false;
			break;
		}

		if (map[tmpTile.X][tmpTile.Y] == '#') {
			//DEBUG
			cout << "Tile at[" << tmpTile.X << "][" << tmpTile.Y << "] is a wall!" << endl;
			return true;
		} else {
			//DEBUG
			cout << "Tile at[" << tmpTile.X << "][" << tmpTile.Y << "] is empty!" << endl;
			return false;
		}
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