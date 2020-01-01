
#include "S2D/S2D.h"

using namespace S2D;


	struct Vector2i {
		int X = 0;
		int Y = 0;

		Vector2i(int newX = 0, int newY = 0) {
			X = newX;
			Y = newY;
		}

		Vector2 get() {
			Vector2 tmp;
			tmp.X = X;
			tmp.Y = Y;

			return tmp;
		}
	};

	enum class Direction : unsigned int {
		dRIGHT = 0,
		dDOWN,
		dLEFT,
		dUP,
		dNULL
	};

	enum class GameState : unsigned int {
		gsPlaying = 0,	// game is playing - all entites are moving
		gsRunning,		// * game is running - pacman not moving?
		gsPausing,		// game is paused - no entities are moving
		gsStopping,		// * game is ending - destroy all objects and end
		gsScoring		// game is showing high scores - no entities are drawn
	};

	enum class Movement : unsigned int {
		mLEFT = 0,
		mRIGHT,
		mUP,
		mDOWN,
		mSTOP
	};


	Direction IsFacing(Movement movement) {
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

	Movement RandomMotion() {
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

