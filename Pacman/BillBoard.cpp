#include "BillBoard.h"

BillBoard::BillBoard() {
	textRect = new Rect();
	textPos = Vector2i();
	pauseKeyDown = false;
	startKeyDown = false;
	caption = "Hullo";
	isPaused = false;

	title = new Texture2D();
}

BillBoard::~BillBoard() {
	delete textRect;
	delete title;
}

void BillBoard::ConfigState(bool justPaused) {
	isPaused = justPaused;
}

void BillBoard::Draw(int frameCount) {
	//re-configure background incase of window resizing
	position = new Rect( 0.0f, 0.0f, 
		Graphics::GetViewportWidth(),
		Graphics::GetViewportHeight()
	);

	// draw background
	SpriteBatch::Draw(texture, position, nullptr);

	//configure text box
	textRect->Width = position->Width / 3;
	textRect->Height = position->Height / 4;
	textRect->X = (position->Width / 2) - (textRect->Width / 2);
	textRect->Y = position->Height / 2;

	if (isPaused) {

		//configure caption
		SetString("PAUSED!");

		textPos.X = textRect->X + (textRect->Width / 3.75);
		textPos.Y = textRect->Y + (textRect->Height / 2);

	} else {
		//configure caption
		SetString("Press [SPACE] to start!");

		textPos.X = textRect->X + (textRect->Width / 9.5);
		textPos.Y = textRect->Y + (textRect->Height / 2);

		//draw title
		DrawTitle();
	}
	
	//draw text box
	SpriteBatch::DrawRectangle(textRect, Color::Blue);
	
	//draw text
	SpriteBatch::DrawString(caption.c_str(), &textPos.get(), Color::Yellow);
}

void BillBoard::DrawTitle() {
	char txt[] = "PACMAN";
	Rect titlePosition = Rect(
		32 * 8,
		32 * 4,
		strlen(txt) * 64,
		64
	);

	titlePosition.X = (position->Width / 2) - (titlePosition.Width / 2);

	Rect titleRect = titlePosition;
	titleRect.X -= 16;
	titleRect.Y += 8;

	//draw title box
	SpriteBatch::DrawRectangle(&titleRect, Color::Blue);

	Rect titleChar = Rect(0, 0, 64, 64);
	Rect titleSource = Rect();
	char ctmp = ' ';
	int chr = 64;					//char conversion ('A' = 65)
	int charSize = 64;				//width and height of chars in texture

	/*
	texture dims
	w = 2048
	h = 512
	chars in texture
	w = 32
	h = 8
	per char dims
	w = 64
	h = 64
	*/
	titleSource.Y = 0;
	titleSource.Width = charSize;
	titleSource.Height= charSize;
	titleChar.Y = titlePosition.Y;
	
	for (int i = 0; i < strlen(txt); i++) {
		titleSource.X = ((int)txt[i] - chr) * charSize;

		titleChar.X = titlePosition.X + (i * titleChar.Width);

		SpriteBatch::Draw(
			title,				//texture
			&titleChar,			//position
			&titleSource,		//position within texture
			Vector2::Zero,		//sprite origin
			1.0f,				//sprite scale
			0.0f,				//sprite rotation
			Color::Yellow,		//colour
			SpriteEffect::NONE	//applied effects (eg. flip)
		);
	}
}

void BillBoard::SetString(string newText) {
	caption = "_. - | " + newText + " | -._";
}