#include "Entity.h"

Entity::Entity() {
	this->sourceRect = new Rect();
	this->position = new Rect();
	this->texture = new Texture2D();

	this->currentFrame = 0;
	this->maxFrame = 0;
	this->delayFrame = 20;
	this->delayCounter = 0;
	this->scale = 1;
	this->offset = 0;
}

Entity::~Entity() {
	delete this->sourceRect;
	delete this->position;
	delete this->texture;
}

void Entity::Draw(int frameCount) {
	Vector2 midPoint = Vector2();
	midPoint.X = this->position->Width / 2;
	midPoint.Y = this->position->Height / 2;

	Rect adjust = Rect(
		this->position->X + this->offset,
		this->position->Y + this->offset,
		this->position->Width,
		this->position->Height
	);

	//Middle
	SpriteBatch::Draw(
		this->texture,		//texture
		&adjust,			//position
		this->sourceRect,	//position within texture
		&midPoint,			//sprite origin
		this->scale,		//sprite scale
		0.0f,				//sprite rotation
		Color::White,		//colour
		SpriteEffect::NONE	//applied effects (eg. flip)
	);
}

int Entity::GetFrame()
{
	return this->currentFrame;
}

float Entity::GetSpeed()
{
	return this->speed;
}

void Entity::SetFrameDelay(int delayBy) {
	this->delayFrame = delayBy;
}

///<summary>Sets number of max frames based on texture width / tilesize param</summary>
void Entity::SetFrames(int tileSize) {
	this->maxFrame = this->texture->GetWidth()/tileSize;
}

void Entity::SetOffset(int newOffset) {
	this->offset = newOffset;
}

void Entity::SetSpeed(float newSpeed)
{
	this->speed = newSpeed;
}

///<summary>Sets number of max frames directly</summary>
void Entity::SetMaxFrames(int numOfFrames) {
	this->maxFrame = numOfFrames - 1;
}

void Entity::SetTexture(Texture2D * source) {
	this->texture = source;
}

void Entity::Update(int elapsedTime){
	if (this->delayCounter > this->delayFrame) {
		this->delayCounter = 0;
		this->NextFrame();
	} else {
		this->delayCounter++;
	}

	if (this->maxFrame > 0) {
		this->sourceRect->X = this->sourceRect->Width * this->currentFrame;
	}
}

void Entity::NextFrame()
{
	if (this->currentFrame < this->maxFrame) {
		this->currentFrame++;
	} else {
		this->currentFrame = 0;
	}
}
