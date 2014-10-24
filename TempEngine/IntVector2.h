#pragma once
#ifndef INTVECTOR2_H
#define INTVECTOR2_H

class IntVector2
{
private:
	inline void Rotate90Degrees( int& x, int& y );
	inline void RotateMinus90Degrees( int& x, int& y );

public:
	int x;
	int y;

	//constructors
	//
	IntVector2();
	IntVector2( const IntVector2& vectorToCopy );
	explicit IntVector2( int x, int y );
	~IntVector2();

	//Setters
	//
	void SetXY( int newXValue, int newYValue );

	//Scalers
	//
	void ScaleUniform( float scale );
	void ScaleNonUniform( float xScale, float yScale );

	//Rotation
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void Reverse();


	void GetXY( int& xHolder, int& yHolder ) const;

	float CalculateLength() const;
	float CalculateLengthSquared() const;

	//Operators
	//
	void operator=( const IntVector2& rhs );
	void operator+=( const IntVector2& rhs );
	void operator-=( const IntVector2& rhs );
	void operator*=( float scale );
	void operator/=( float inverseScale );
	IntVector2 operator+( const IntVector2& rhs ) const;
	IntVector2 operator-( const IntVector2& rhs ) const;
	IntVector2 operator-() const;
	IntVector2 operator*( float scale ) const;
	IntVector2 operator/( float inverseScale ) const;
	bool operator==( const IntVector2& rhs ) const;
	bool operator!=( const IntVector2& rhs ) const;

	float DotProduct( const IntVector2& vectorB ) const;

};

IntVector2 operator*(const float scale, const IntVector2& vector);

inline void IntVector2::Rotate90Degrees( int& x, int& y )
{
	int tempX = x;

	x = -y;
	y = tempX;
}

inline void IntVector2::RotateMinus90Degrees( int& x, int& y )
{
	int tempY = y;

	y = -x;
	x = tempY;
}

#endif