#include "Vector2i.h"

const Vector2i* Vector2i::Zero = new Vector2i();
const Vector2i* Vector2i::One = new Vector2i(1.0f, 1.0f);
const Vector2i* Vector2i::UnitX = new Vector2i(1.0f, 0.0f);
const Vector2i* Vector2i::UnitY = new Vector2i(1.0f, 0.0f);

Vector2i::Vector2i(int newX, int newY)
{
	this->X = newX;
	this->Y = newY;
}

bool Vector2i::operator== (const Vector2i& rhs) const
{
	if (this->X == rhs.X && this->Y == rhs.Y)
		return true;
	else
		return false;
}

bool Vector2i::operator!=(const Vector2i& other) const
{
	return !(*this == other);
}

Vector2i& Vector2i::operator/=(const Vector2i& rhs)
{
	this->X /= rhs.X;
	this->Y /= rhs.Y;

	return *this;
}

Vector2i& Vector2i::operator*=(const Vector2i& rhs)
{
	this->X *= rhs.X;
	this->Y *= rhs.Y;

	return *this;
}

Vector2i& Vector2i::operator/=(const int& rhs)
{
	this->X /= rhs;
	this->Y /= rhs;

	return *this;
}

Vector2i& Vector2i::operator*=(const int& rhs)
{
	this->X *= rhs;
	this->Y *= rhs;

	return *this;
}

Vector2i& Vector2i::operator= (const Vector2i& rhs)
{
	this->X = rhs.X;
	this->Y = rhs.Y;
	return *this;
}

Vector2i& Vector2i::operator+=(const Vector2i& rhs)
{
	this->X += rhs.X;
	this->Y += rhs.Y;

	return *this;
}

Vector2i& Vector2i::operator-=(const Vector2i& rhs)
{
	this->X -= rhs.X;
	this->Y -= rhs.Y;

	return *this;
}

const Vector2i Vector2i::operator+(const Vector2i& other) const
{
	Vector2i result = *this;
	result += other;
	return result;
}

const Vector2i Vector2i::operator-(const Vector2i& other) const
{
	Vector2i result = *this;
	result -= other;
	return result;
}

const Vector2i Vector2i::operator*(const Vector2i& other) const
{
	Vector2i result = *this;
	result *= other;
	return result;
}

const Vector2i Vector2i::operator/(const Vector2i& other) const
{
	Vector2i result = *this;
	result /= other;
	return result;
}

const Vector2i Vector2i::operator*(const int& other) const
{
	Vector2i result = *this;
	result *= other;
	return result;
}

const Vector2i Vector2i::operator/(const int& other) const
{
	Vector2i result = *this;
	result /= other;
	return result;
}

int Vector2i::Length() const
{
	return sqrtf(pow(this->X, 2) + pow(this->Y, 2));
}

int Vector2i::LengthSquared() const
{
	return pow(this->X, 2) + pow(this->Y, 2);
}

void Vector2i::Normalize()
{
	int length = this->Length();
	this->X /= length;
	this->Y /= length;
}

Vector2i Vector2i::Clamp(const Vector2i& value1, const Vector2i& min, const Vector2i& max)
{
	Vector2i result;

	if (value1.X < min.X)
		result.X = min.X;
	if (value1.Y < min.Y)
		result.Y = min.Y;
	if (value1.X > max.X)
		result.X = max.X;
	if (value1.Y > max.Y)
		result.Y = max.Y;

	return result;
}

void Vector2i::Clamp(const Vector2i& value1, const Vector2i& min, const Vector2i& max, Vector2i& result)
{
	if (value1.X < min.X)
		result.X = min.X;
	if (value1.Y < min.Y)
		result.Y = min.Y;
	if (value1.X > max.X)
		result.X = max.X;
	if (value1.Y > max.Y)
		result.Y = max.Y;
}

int Vector2i::Distance(const Vector2i& value1, const Vector2i& value2)
{
	return (value1 - value2).Length();
}

void Vector2i::Distance(const Vector2i& value1, const Vector2i& value2, int& result)
{
	result = (value1 - value2).Length();
}

int Vector2i::DistanceSquared(const Vector2i& value1, const Vector2i& value2)
{
	return (value1 - value2).LengthSquared();
}

void Vector2i::DistanceSquared(const Vector2i& value1, const Vector2i& value2, int& result)
{
	result = (value1 - value2).LengthSquared();
}

int Vector2i::Dot(const Vector2i& value1, const Vector2i& value2)
{
	return (value1.X * value2.X) + (value1.Y * value2.Y);
}

void Vector2i::Dot(const Vector2i& value1, const Vector2i& value2, int& result)
{
	result = (value1.X * value2.X) + (value1.Y * value2.Y);
}

S2D::Vector2 Vector2i::get() {
	S2D::Vector2 tmp;
	tmp.X = X;
	tmp.Y = Y;

	return tmp;
}

Vector2i Vector2i::Lerp(const Vector2i& value1, const Vector2i& value2, const int amount)
{
	return value1 + (value2 - value1) * amount;
}

void Vector2i::Lerp(const Vector2i& value1, const Vector2i& value2, const int amount, Vector2i& result)
{
	result = value1 + (value2 - value1) * amount;
}

Vector2i Vector2i::Max(const Vector2i& value1, const Vector2i& value2)
{
	Vector2i result;

	result.X = value1.X > value2.X ? value1.X : value2.X;
	result.Y = value1.Y > value2.Y ? value1.Y : value2.Y;

	return result;
}

void Vector2i::Max(const Vector2i& value1, const Vector2i& value2, Vector2i& result)
{
	result.X = value1.X > value2.X ? value1.X : value2.X;
	result.Y = value1.Y > value2.Y ? value1.Y : value2.Y;
}

Vector2i Vector2i::Min(const Vector2i& value1, const Vector2i& value2)
{
	Vector2i result;

	result.X = value1.X < value2.X ? value1.X : value2.X;
	result.Y = value1.Y < value2.Y ? value1.Y : value2.Y;

	return result;
}

void Vector2i::Min(const Vector2i& value1, const Vector2i& value2, Vector2i& result)
{
	result.X = value1.X < value2.X ? value1.X : value2.X;
	result.Y = value1.Y < value2.Y ? value1.Y : value2.Y;
}

Vector2i Vector2i::Negate(const Vector2i& value)
{
	return Vector2i(-value.X, -value.Y);
}

void Vector2i::Negate(const Vector2i& value, Vector2i& result)
{
	result.X = -value.X;
	result.Y = -value.Y;
}

Vector2i Vector2i::Normalize(const Vector2i& value)
{
	Vector2i result;
	int length = value.Length();
	result.X = value.X / length;
	result.Y = value.Y / length;
	return result;
}

void Vector2i::Normalize(const Vector2i& value, Vector2i& result)
{
	int length = value.Length();
	result.X = value.X / length;
	result.Y = value.Y / length;
}

Vector2i Vector2i::Reflect(const Vector2i& vector, const Vector2i& normal)
{
	//R = V - 2 * (V . N) * N
	Vector2i result;
	int temp;
	Vector2i::Dot(vector, normal, temp);

	result = vector - normal * temp * 2;

	return result;
}

void Vector2i::Reflect(const Vector2i& vector, const Vector2i& normal, Vector2i& result)
{
	int temp;
	Vector2i::Dot(vector, normal, temp);

	result = vector - normal * temp * 2;
}

