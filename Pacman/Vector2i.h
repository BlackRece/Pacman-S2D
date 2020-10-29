#ifndef VECTOR2I_H
#define VECTOR2I_H

#include "S2D/S2D.h"

class Vector2i
{
public:
	/// <summary>	The X coordinate. </summary>
	int X;

	/// <summary>	The Y coordinate. </summary>
	int Y;

	/// <summary>Creates a Vector Structure (X=0, Y=0)
	/// <param name="x">X Attribute</param>
	/// <param name="y">Y Attribute</param>
	/// </summary>
	Vector2i(int newX = 0, int newY = 0);

	S2D::Vector2 get();
	
	/// <summary>	Assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	A shallow copy of this object. </returns>
	Vector2i& operator= (const Vector2i& rhs);

	/// <summary>	Equality operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	true if the parameters are considered equivalent. </returns>
	bool operator== (const Vector2i& rhs) const;

	/// <summary>	Inequality operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	true if the parameters are not considered equivalent. </returns>
	bool operator!=(const Vector2i& other) const;

	/// <summary>	Addition assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator+=(const Vector2i& rhs);

	/// <summary>	Subtraction assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator-=(const Vector2i& rhs);

	/// <summary>	Division assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator/=(const Vector2i& rhs);

	/// <summary>	Multiplication assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator*=(const Vector2i& rhs);

	/// <summary>	Division assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator/=(const int& rhs);

	/// <summary>	Multiplication assignment operator. </summary>
	///
	/// <param name="rhs">	The right hand side. </param>
	///
	/// <returns>	The result of the operation. </returns>
	Vector2i& operator*=(const int& rhs);

	/// <summary>	Addition operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator+(const Vector2i& other) const;

	/// <summary>	Negation operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator-(const Vector2i& other) const;

	/// <summary>	Multiplication operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator*(const Vector2i& other) const;

	/// <summary>	Division operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator/(const Vector2i& other) const;

	const Vector2i operator%(const Vector2i& other) const;

	const Vector2i operator%(const int& rhs) const;

	/// <summary>	Multiplication operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator*(const int& other) const;

	/// <summary>	Division operator. </summary>
	///
	/// <param name="other">	The other. </param>
	///
	/// <returns>	The result of the operation. </returns>
	const Vector2i operator/(const int& other) const;

	/// <summary>	Gets the length. </summary>
	///
	/// <returns>	The Vector Length. </returns>
	int Length() const;

	/// <summary>	Gets the length squared. </summary>
	///
	/// <returns>	The Vector length squared. </returns>
	int LengthSquared() const;

	/// <summary>	Normalizes this Vector. </summary>
	void Normalize();

	/// <summary>	Clamp the given value. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="min">   	The minimum. </param>
	/// <param name="max">   	The maximum. </param>
	///
	/// <returns>	A Vector2. </returns>
	static Vector2i Clamp(const Vector2i& value1, const Vector2i& min, const Vector2i& max);

	/// <summary>	Clamp the given value. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="min">   	The minimum. </param>
	/// <param name="max">   	The maximum. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Clamp(const Vector2i& value1, const Vector2i& min, const Vector2i& max, Vector2i& result);

	/// <summary>	Finds the distance between two Vectors. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	///
	/// <returns>	The distance. </returns>
	static int Distance(const Vector2i& value1, const Vector2i& value2);

	/// <summary>	Finds the distance between two Vectors. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Distance(const Vector2i& value1, const Vector2i& value2, int& result);

	/// <summary>	Finds the distance between two Vectors squared. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	///
	/// <returns>	A float. </returns>
	static int DistanceSquared(const Vector2i& value1, const Vector2i& value2);

	/// <summary>	Finds the distance between two Vectors squared. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void DistanceSquared(const Vector2i& value1, const Vector2i& value2, int& result);

	/// <summary>	Performs the Dot Product between two vectors. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	///
	/// <returns>	The Dot Product. </returns>
	static int Dot(const Vector2i& value1, const Vector2i& value2);

	/// <summary>	Performs the Dot Product between two vectors. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Dot(const Vector2i& value1, const Vector2i& value2, int& result);

	/// <summary>	Linearly Interpolates between two Vectors by the given amount. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="amount">	The amount. </param>
	///
	/// <returns>	A Vector2. </returns>
	static Vector2i Lerp(const Vector2i& value1, const Vector2i& value2, int amount);

	/// <summary>	Linearly Interpolates between two Vectors by the given amount. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="amount">	The amount. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Lerp(const Vector2i& value1, const Vector2i& value2, int amount, Vector2i& result);

	/// <summary>	Determines the maximum of the given parameters. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	///
	/// <returns>	The maximum value. </returns>
	static Vector2i Max(const Vector2i& value1, const Vector2i& value2);

	/// <summary>	Determines the maximum of the given parameters. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Max(const Vector2i& value1, const Vector2i& value2, Vector2i& result);

	/// <summary>	Determines the minimum of the given parameters. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	///
	/// <returns>	The minimum value. </returns>
	static Vector2i Min(const Vector2i& value1, const Vector2i& value2);

	/// <summary>	Determines the minimum of the given parameters. </summary>
	///
	/// <param name="value1">	The first value. </param>
	/// <param name="value2">	The second value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Min(const Vector2i& value1, const Vector2i& value2, Vector2i& result);

	/// <summary>	Negates the given value. </summary>
	///
	/// <param name="value">	The value. </param>
	///
	/// <returns>	A Vector2. </returns>
	static Vector2i Negate(const Vector2i& value);

	/// <summary>	Negates the given value. </summary>
	///
	/// <param name="value"> 	The value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Negate(const Vector2i& value, Vector2i& result);

	/// <summary>	Normalizes the given value. </summary>
	///
	/// <param name="value">	The value. </param>
	///
	/// <returns>	A Vector2. </returns>
	static Vector2i Normalize(const Vector2i& value);

	/// <summary>	Normalizes the given value. </summary>
	///
	/// <param name="value"> 	The value. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Normalize(const Vector2i& value, Vector2i& result);

	/// <summary>	Reflects. </summary>
	///
	/// <param name="vector">	The vector. </param>
	/// <param name="normal">	The normal. </param>
	///
	/// <returns>	A Vector2. </returns>
	static Vector2i Reflect(const Vector2i& vector, const Vector2i& normal);

	/// <summary>	Reflects. </summary>
	///
	/// <param name="vector">	The vector. </param>
	/// <param name="normal">	The normal. </param>
	/// <param name="result">	[in,out] The result. </param>
	static void Reflect(const Vector2i& vector, const Vector2i& normal, Vector2i& result);

	/// <summary>	Zero Vector. </summary>
	static const Vector2i* Zero;
	/// <summary>	One Vector. </summary>
	static const Vector2i* One;
	/// <summary>	UnitX Vector. </summary>
	static const Vector2i* UnitX;
	/// <summary>	UnitY Vector. </summary>
	static const Vector2i* UnitY;
};

#endif // VECTOR2I_H