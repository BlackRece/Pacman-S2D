#include "Entity.h"

Entity::Entity() {
	sourceRect = new Rect();
	position = new Rect();
	texture = new Texture2D();
	
	currentFrame = 0;
	maxFrame = 0;
	delayFrame = 20;
	delayCounter = 0;
	scale = 1;
	offset = 0;

	speed = 0;
}

Entity::~Entity() {
	delete sourceRect;
	delete position;
	delete texture;
}

void Entity::Draw(int frameCount) {
	Vector2 midPoint = Vector2();
	midPoint.X = position->Width / 2;
	midPoint.Y = position->Height / 2;

	Rect adjust = Rect(
		position->X + offset,
		position->Y + offset,
		position->Width,
		position->Height
	);

	//Middle
	SpriteBatch::Draw(
		texture,		//texture
		&adjust,			//position
		sourceRect,	//position within texture
		&midPoint,			//sprite origin
		scale,		//sprite scale
		0.0f,				//sprite rotation
		Color::White,		//colour
		SpriteEffect::NONE	//applied effects (eg. flip)
	);
}

int Entity::GetFrame()
{
	return currentFrame;
}

int Entity::GetMaxFrames() {
	return maxFrame + 1;
}

float Entity::GetSpeed()
{
	return speed;
}

Vector2* Entity::GetPosFloat() {
	return new Vector2(position->X, position->Y);
}

Vector2i* Entity::GetPosInt() {
	return new Vector2i(floor(position->X), floor(position->Y));
}

void Entity::MoveBy(Vector2 velocity) {
	position->X += velocity.X;
	position->Y += velocity.Y;
}

void Entity::SetPosInt(Vector2i newPos) {
	position->X = newPos.X;
	position->Y = newPos.Y;
}

void Entity::SetPosFloat(Vector2 newPos) {
	position->X = newPos.X;
	position->Y = newPos.Y;
}

void Entity::SetFrameDelay(int delayBy) {
	delayFrame = delayBy;
}

///<summary>Sets number of max frames based on texture width / tilesize param</summary>
void Entity::SetFrames(int tileSize) {
	maxFrame = texture->GetWidth()/tileSize;
}

void Entity::SetOffset(int newOffset) {
	offset = newOffset;
}

void Entity::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

///<summary>Sets number of max frames directly</summary>
void Entity::SetMaxFrames(int numOfFrames) {
	maxFrame = numOfFrames - 1;
}

void Entity::SetTexture(Texture2D * source) {
	texture = source;
}

void Entity::Update(int elapsedTime){
	UpdateFrame();

	if (maxFrame > 0) {
		sourceRect->X = sourceRect->Width * currentFrame;
	}
}

void Entity::UpdateFrame() {
	if (delayCounter > delayFrame) {
		delayCounter = 0;
		NextFrame();
	} else {
		delayCounter++;
	}
}

void Entity::NextFrame()
{
	if (currentFrame < maxFrame) {
		currentFrame++;
	} else {
		currentFrame = 0;
	}
}
