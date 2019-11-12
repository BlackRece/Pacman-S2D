#include "Pacman.h"
#include <sstream>
#include <iostream>
#include <math.h>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250)
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
	_pacman->currentMove = Movement::mSTOP;
	_pacman->nextMove = Movement::mSTOP;
	_pacman->lastMove = Movement::mSTOP;
	_pacman->facing = Direction::dLEFT;
	_pacman->speed = 0.15f;
	_pacman->score = 0;

	// initialise walls
	_wallTexture = new Texture2D();
	_wallCounter = 0;
	_wallScale = 1.0f;

	// initialise munchies
	_munchieTexture = new Texture2D();
	_munchieCounter = 0;
	
	for (int row = 0; row < map.size(); row++) {
		for (int col = 0; col < map[row].size(); col++) {
			switch(map.at(row).at(col)){
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
				_munchies[_munchieCounter] = new entity();
				_munchies[_munchieCounter]->sourceRect = new Rect(0.0f, 0.0f, 11, 11);
				_munchies[_munchieCounter]->posRect = new Rect(
					(col ) * TILE_SIZE, 
					(row ) * TILE_SIZE,
					12, 12
				);
				
				_munchies[_munchieCounter]->isEaten = false;

				//power munchies
				if (map.at(row).at(col) == '+') {
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
				_munchies[_munchieCounter]->posRect->X +=
					_munchies[_munchieCounter]->offset - _munchies[_munchieCounter]->posRect->Width / 2;
				_munchies[_munchieCounter]->posRect->Y +=
					_munchies[_munchieCounter]->offset - _munchies[_munchieCounter]->posRect->Height / 2;

				_munchieCounter++;
				break;
			case '#':
				_walls[_wallCounter] = new Obstacle();
				_walls[_wallCounter]->sourceRect = new Rect(0.0f, 0.0f, TILE_SIZE, TILE_SIZE);
				_walls[_wallCounter]->posRect = new Rect(
					(col ) * TILE_SIZE,
					(row )* TILE_SIZE, 
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
		"\n     X: " << _munchies[_munchieCounter-1]->posRect->X <<
		"\n     Y: " << _munchies[_munchieCounter-1]->posRect->Y <<
		"\nHeight: " << _munchies[_munchieCounter-1]->posRect->Height <<
		"\n Width: " << _munchies[_munchieCounter-1]->posRect->Width << endl << endl;
	cout << "wall number = " << _wallCounter << endl;
	cout << "_walls[_wallCounter]->rect;" <<
		"\n     X: " << _walls[_wallCounter - 1]->posRect->X <<
		"\n     Y: " << _walls[_wallCounter - 1]->posRect->Y <<
		"\nHeight: " << _walls[_wallCounter - 1]->posRect->Height <<
		"\n Width: " << _walls[_wallCounter - 1]->posRect->Width << endl;

	_paused = true;
	_pKeyDown = false;
	_mainMenu = true;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, _wWidth, _wHeight, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	DeletePlayer(_pacman);
	delete _pacman;

	for (int i = 0; i < _munchieCounter; i++) {
		DeleteEntity(_munchies[i]);
	}
	delete _munchies;
	delete _munchieTexture;
	
	for (int i = 0; i < _wallCounter; i++) {
		DeleteObstacle(_walls[i]);
	}
	delete _walls;
	delete _wallTexture;
}

void Pacman::DeletePlayer(Player *obj) {
	delete obj->texture;
	delete obj->position;
	//delete obj->rect;
	delete obj->sourceRect;
	delete obj->direction;
}

void Pacman::DeleteEntity(entity* obj) {
	delete obj->posRect; 
	delete obj->sourceRect;
}

void Pacman::DeleteObstacle(Obstacle* obj) {
	delete obj->posRect;
	delete obj->sourceRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture->Load("Textures/Pacman.tga", false);

	// Load Munchie
	_munchieTexture->Load("Textures/Munchie.png", true);
	
	// Load Walls
	_wallTexture->Load("Textures/wallTile.tga", true);

	// Set Menu Parameters
	_menuBackGround = new Texture2D();
	_menuBackGround->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, _wWidth, _wHeight);
	_menuTextRect = new Rect(
		_wWidth / 4.0f,
		(_wHeight / 2.0f) + 50.0f,
		_wWidth / 2.0f,
		100.0f);
	_menuStringPosition = new Vector2(_wWidth / 2.3f,
		(_wHeight / 2.0f) + 100.0f);
	
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

void Pacman::Update(int elapsedTime) {
	/* TODO:
	 * =====
	 *
	 * - AI movement for enemies/ghosts
	 * - Timed spawn of Fruit/PickUp
	 * - Update of score? (on player collision may be better
	*/

	/* PACMAN UPDATE */
	std::cout << "elapsedTime : " << elapsedTime << "ms" << std::endl;
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	CheckPaused(keyboardState, Input::Keys::P, Input::Keys::SPACE);

	if (!_paused) {
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
		"CurrentMovement: " << GetMovementString(_pacman->currentMove) <<
		" NextMovement: " << GetMovementString(_pacman->nextMove);
	streamScore << "Score : " << _pacman->score;

	SpriteBatch::BeginDraw(); // Starts Drawing

	// Draws Pacman
	SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); 

	// Draws Munchies
	for (int i = 0; i < _munchieCounter; i++) {
		if (!_munchies[i]->isEaten) {
			SpriteBatch::Draw(_munchieTexture, _munchies[i]->posRect, _munchies[i]->sourceRect, Vector2::Zero, _munchies[i]->scale, 0.0f, Color::White, SpriteEffect::NONE);
		}
	}

	// Draw Walls
	for (int i = 0; i < _wallCounter; i++) {
		SpriteBatch::Draw(_wallTexture, _walls[i]->posRect, _walls[i]->sourceRect, Vector2::Zero, _wallScale, 0.0f, Color::White, SpriteEffect::NONE);
	}

	// Reset animation loop
	if (_frameCount >= 60) {
		_frameCount = 0;
	}
	
	if (_paused) {
		std::stringstream menuStream;
		if (_mainMenu) {
			menuStream << "_.-| Press SPACE BAR to start |-._";
			_menuStringPosition->X = _wWidth / 2.5f;
		} else {
			menuStream << "_.-| PAUSED! |-._";
			_menuStringPosition->X = _wWidth / 2.3f;
		}

		SpriteBatch::Draw(_menuBackGround, _menuRectangle, nullptr);
		SpriteBatch::DrawRectangle(_menuTextRect, Color::Red);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Green);
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
	if (!_paused) {
		//Set pacman's LAST movement
		_pacman->lastMove = _pacman->currentMove;

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
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey, Input::Keys startKey){
	// Checks if ESCAPE key is pressed
	if (state->IsKeyDown(Input::Keys::ESCAPE)) {
		Graphics::Destroy();
	}

	// Checks is StartGame key is pressed (Start Screen)
	if (state->IsKeyDown(startKey)) {
		if (_mainMenu && _paused) {
			_mainMenu = false;
			_paused = false;
		}
	}

	// Checks if Pause key is pressed (Pause Screen)
	if (!_pKeyDown && state->IsKeyDown(pauseKey)) {
		_paused = !_paused;
		_pKeyDown = true;
	}

	// 
	if (state->IsKeyUp(pauseKey)) {
		_pKeyDown = false;
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
	for (int i = 0; i < _munchieCounter; i++) {
		if (!_munchies[i]->isEaten) {
			//test munchie position
			if (HasTargetHitObject(&tmpRect, _munchies[i]->posRect, _pacman->speed*2)) {
				//set flag
				_munchies[i]->isEaten = true;

				//update score
				_pacman->score += _munchies[i]->value;

				//DEBUG
				cout << "Eaten munchie at X:" << _munchies[i]->posRect->X <<
					" Y:" << _munchies[i]->posRect->Y << endl;
			}
		}
	}

	//check collision with walls
	//HasHitWall();
}

bool Pacman::HasHitWall(Rect* target, bool isPlayer = true,
	float targetTolerance, float wallTolerance) {
	//checks for player or ghosts
	bool passThroughable;

	//add buffer to walls
	Rect wall;
	float slim = wallTolerance;	//reduction in wall boundary
	
	for (int i = 0; i < _wallCounter; i++) {
		if (isPlayer) {
			passThroughable = _walls[i]->canPlayerPass;
		} else {
			passThroughable = _walls[i]->canGhostsPass;
		}
		if (!passThroughable) {
			wall = *_walls[i]->posRect;
			wall.Width -= slim;
			wall.Height -= slim;
			wall.X += slim / 2;
			wall.Y += slim / 2;
			if (HasTargetHitObject(target, &wall, targetTolerance)) {
				return true;
			}
		}
	}

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

bool Pacman::HasTargetHitObject(Rect* git, Rect* obj, float tolerance) {
	float slim = tolerance;
	float gitRadiusX = (git->Width - slim) / 2;
	float gitRadiusY = (git->Height - slim) / 2;
	float gitCenterX = git->X + gitRadiusX + (slim / 2);
	float gitCenterY = git->Y + gitRadiusY + (slim / 2);

	//find center and radius of obj
	float objCenterX, objCenterY;
	float objRadiusX, objRadiusY;

	objRadiusX = obj->Width / 2;
	objRadiusY = obj->Height / 2;
	objCenterX = obj->X + objRadiusX;
	objCenterY = obj->Y + objRadiusY;

	//test munchie position
	if (abs(gitCenterX - objCenterX) < gitRadiusX + objRadiusX &&
		abs(gitCenterY - objCenterY) < gitRadiusY + objRadiusY) {
		return true;
	}
	else {
		return false;
	}
}

void Pacman::CheckViewportCollision(){
	/* wrap-around */
	if (_pacman->position->Y < 0.0f) {
		_pacman->position->Y = _wHeight - _pacman->sourceRect->Height;
	}

	if (_pacman->position->Y > _wHeight - _pacman->sourceRect->Height) {
		_pacman->position->Y = 1.0f;
	}

	if (_pacman->position->X < 1.0f) {
		_pacman->position->X = _wWidth - _pacman->sourceRect->Width;
	}

	if (_pacman->position->X > _wWidth - _pacman->sourceRect->Width) {
		_pacman->position->X = 1.0f;
	}
}

void Pacman::UpdatePacman(int elapsedTime){
	bool nextMoveValid = false;
	bool currMoveValid = false;
	bool canTurn = false;
	Rect tmpRect = Rect();
	Vector2 tmpDir = Vector2();
	Vector2 nextDir = Vector2();
	Vector2 currDir = Vector2();
	Vector2 lastDir = Vector2();
	float tmpSpeed = _pacman->speed * elapsedTime;

	//get player pos
	Vector2i tmpPos;
	tmpPos.X = (int)round(_pacman->position->X);
	tmpPos.Y = (int)round(_pacman->position->Y);

	if (tmpPos.X % TILE_SIZE == 0) {
		_pacman->position->X = tmpPos.X;
	}
	if (tmpPos.Y % TILE_SIZE == 0) {
		_pacman->position->Y = tmpPos.Y;
	}

	if (tmpPos.X % TILE_SIZE == 0 &&
		tmpPos.Y % TILE_SIZE == 0) {
		canTurn = true;
	}
	else {
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
	//_pacman->sourceRect->X = (_pacman->sourceRect->Width * _pacman->frame)+5;

	//Get player position and size
	tmpRect = Rect(
		_pacman->position->X,
		_pacman->position->Y,
		_pacman->sourceRect->Width,
		_pacman->sourceRect->Height
	);

	//Get direction vector
	nextDir = WillHitWall(&tmpRect, _pacman->nextMove, tmpSpeed);

	//Is NextMove valid ? or does NextMove hit a wall ?
	if (nextDir.X < 0.0f || nextDir.X > 0.0f ||
		nextDir.Y < 0.0f || nextDir.Y > 0.0f) {
		if (canTurn) {
			tmpDir = nextDir;
			_pacman->currentMove = _pacman->nextMove;
		}
	} else {
		//DEBUG
		std::cout << "Next Move Hits a wall." << endl;
		//wait until able to apply NextMove
	}

	if (_pacman->currentMove == Movement::mSTOP) {
		//overwrite current move with next move
		currDir = nextDir;
		_pacman->currentMove = _pacman->nextMove;
	} else {
		if (nextDir.X < 0.0f || nextDir.X > 0.0f ||
			nextDir.Y < 0.0f || nextDir.Y > 0.0f) {
			currDir = nextDir;
			_pacman->currentMove = _pacman->nextMove;
		} else {
			//Is CurrentMove valid? or does CurrentMove hit a wall?
			currDir = WillHitWall(&tmpRect, _pacman->currentMove, tmpSpeed);

			if (currDir.X < 0.0f || currDir.X > 0.0f ||
				currDir.Y < 0.0f || currDir.Y > 0.0f) {
				tmpDir = currDir;
			}
			else {
				//DEBUG
				std::cout << "Curr Move Hits a wall." << endl;

				//reset player to tilemap
				if (!canTurn) {
					tmpPos.X = (int)round(_pacman->position->X / TILE_SIZE);
					tmpPos.Y = (int)round(_pacman->position->Y / TILE_SIZE);

					_pacman->position->X = tmpPos.X * TILE_SIZE;
					_pacman->position->Y = tmpPos.Y * TILE_SIZE;
				}
				else {
					if (nextDir.X < 0.0f || nextDir.X > 0.0f ||
						nextDir.Y < 0.0f || nextDir.Y > 0.0f) {
						currDir = nextDir;
						_pacman->currentMove = _pacman->nextMove;
					}
				}
			}
		}
	}

	if (_pacman->lastMove != Movement::mSTOP &&
		_pacman->currentMove != Movement::mSTOP) {
		//Is LastMove valid? or does LastMove hit a wall?
		lastDir = WillHitWall(&tmpRect, _pacman->lastMove, tmpSpeed);

		if (lastDir.X < 0.0f || lastDir.X > 0.0f ||
			lastDir.Y < 0.0f || lastDir.Y > 0.0f) {
			currDir = lastDir;
			_pacman->currentMove = _pacman->lastMove;
		} else {
			//DEBUG
			std::cout << "Last Move Hits a wall." << endl;
			_pacman->currentMove = Movement::mSTOP;
		}
	}
	
	tmpDir = currDir;

	////Is NextMove valid? or does NextMove hit a wall?
	//if (nextDir.X < 0.0f || nextDir.X > 0.0f ||
	//	nextDir.Y < 0.0f || nextDir.Y > 0.0f) {
	//	_pacman->currentMove = _pacman->nextMove;
	//	tmpDir = nextDir;
	//} else {
	//	if (_pacman->nextMove != Movement::mSTOP) {
	//		//DEBUG
	//		cout << "Next Move Hits a WALL!!" << endl;

	//		//Is CurrentMove valid? or does CurrentMove hit a wall?
	//		currDir = WillHitWall(&tmpRect, _pacman->currentMove, tmpSpeed);
	//		if (currDir.X < 0.0f || currDir.X > 0.0f ||
	//			currDir.Y < 0.0f || currDir.Y > 0.0f) {
	//			tmpDir = currDir;
	//		}
	//		else {
	//			//DEBUG
	//			cout << "Curr Move Hits a wall!" << endl;

	//			if (_pacman->lastMove != Movement::mSTOP) {
	//				//Is LastMove valid? or does lastMove hit a wall?
	//				lastDir = WillHitWall(&tmpRect, _pacman->lastMove, tmpSpeed);
	//				if (lastDir.X < 0.0f || currDir.X > 0.0f ||
	//					lastDir.Y < 0.0f || lastDir.Y > 0.0f) {
	//					currDir = lastDir;
	//				}
	//				else {
	//					//DEBUG
	//					cout << "Last Move Hits a wall!" << endl;
	//					_pacman->currentMove = Movement::mSTOP;
	//				}

	//			}
	//			else {
	//				//_pacman->currentMove = Movement::mSTOP;
	//			}
	//			tmpDir = currDir;
	//		}
	//	}
	//}

	
	// apply tested direction to pacman's direction
	_pacman->direction->X = tmpDir.X;
	_pacman->direction->Y = tmpDir.Y;

	// apply facing direction to sprite
	if (_pacman->facing != IsFacing(_pacman->currentMove) &&
		IsFacing(_pacman->currentMove) != dNULL) {
		_pacman->facing = IsFacing(_pacman->currentMove);
	}
	//_pacman->sourceRect->Y = (_pacman->sourceRect->Height * _pacman->facing)+3;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->facing;

	// apply direction to position
	_pacman->position->Y += _pacman->direction->Y;
	_pacman->position->X += _pacman->direction->X;
		
	// store last movement
	_pacman->lastMove = _pacman->currentMove;
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

Vector2 Pacman::WillHitWall(Rect* target, Movement targetMove, float targetSpd) {
	/* TODO:

	  Set new direction flag
	  Set current direction flag
	  ?store current player location as old player location?
	  store tmp player location as new direction applied to current player location
	  test tmp player location for collision
	  if tmp player location hits wall then
		?set new player location to old player location?
		remember new direction for next frame
	  else
		set new direction to current direction

	*/

	Vector2 tmpDir = Vector2();

	switch (targetMove) {
	case Movement::mDOWN:
		//Moves Pacman Down Y axis
		tmpDir.Y = targetSpd;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mUP:
		//Moves Pacman up Y axis
		tmpDir.Y = -targetSpd;

		//Resets X axis
		tmpDir.X = 0.0f;
		break;
	case Movement::mLEFT:
		//Moves Pacman Left X axis
		tmpDir.X = -targetSpd;

		//Resets X axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mRIGHT:
		//Moves Pacman Right X axis
		tmpDir.X = targetSpd;

		//Resets Y axis
		tmpDir.Y = 0.0f;
		break;
	case Movement::mSTOP:
		//target stops moving
		//reset both axis
		tmpDir.Y = 0.0f;
		tmpDir.X = 0.0f;
	default:
		return tmpDir;
		break;
	}

	//DEBUG
	target->X += tmpDir.X;
	target->Y += tmpDir.Y;

	if (HasHitWall(target, true)) {
		//target stops moving
		//reset both axis
		tmpDir.Y = 0.0f;
		tmpDir.X = 0.0f;
	}
	return tmpDir;
}

void Pacman::UpdateMunchie(int elapsedTime){
	// Draw Munchies (big and small)
	for (int i = 0; i < _munchieCounter; i++) {
		// Animate munchie
		if (_frameCount < 30) {
			// Select Red Munchie
			_munchies[i]->sourceRect->X = _munchies[i]->posRect->Width;
		} else {
			// Select Blue Munchie
			_munchies[i]->sourceRect->X = 0.0f;
		}
	}
}