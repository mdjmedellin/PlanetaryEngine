#include "MathUtilities.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.h"
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>


int GetNearestInt(float f)
{
	return static_cast< int > ( floor( f + .5f) );
}

float RoundFloatToNearestInt(float f)
{
	return floor ( f + .5f );
}

float GetMaxFloat(float a, float b, float c)
{
	return GetMaxFloat(a,GetMaxFloat(b,c));
}

float GetMaxFloat(float a, float b, float c, float d)
{
	return GetMaxFloat(GetMaxFloat(a,b),GetMaxFloat(c,d));
}

float GetMinFloat(float a, float b, float c)
{
	return GetMinFloat(a,GetMinFloat(b,c));
}

float GetMinFloat(float a, float b, float c, float d)
{
	return GetMinFloat(GetMinFloat(a,b),GetMinFloat(c,d));
}

int GetMaxInt(int a, int b, int c)
{
	return GetMaxInt(a,GetMaxInt(b,c));
}

int GetMaxInt(int a, int b, int c, int d)
{
	return GetMaxInt(GetMaxInt(a,b),GetMaxInt(c,d));
}

int GetMinInt(int a, int b, int c)
{
	return GetMinInt(a,GetMinInt(b,c));
}

int GetMinInt(int a, int b, int c, int d)
{
	return GetMinInt(GetMinInt(a,b),GetMinInt(c,d));
}

float ClampFloatWithinRange(float minimumFloatValue, float maximumFloatValue, float floatToClamp)
{
	if( floatToClamp < minimumFloatValue )
	{
		return minimumFloatValue;
	}
	else if( floatToClamp > maximumFloatValue )
	{
		return maximumFloatValue;
	}

	return floatToClamp;
}

int ClampIntWithinRange(int minimumIntValue, int maximumIntValue, int intToClamp)
{
	if(intToClamp <= maximumIntValue)
	{
		if(intToClamp >= minimumIntValue)
		{
			return intToClamp;
		}

		return minimumIntValue;
	}

	return maximumIntValue;
}

float ConvertDegreesToRadians(float degrees)
{
	return (degrees * ( PI / 180.f));
}

float ConvertRadiansToDegrees(float radians)
{
	return (radians * (180.f / PI));
}

float CalcAngularDistanceDegrees(float degreesA, float degreesB)
{
	float angularDistanceDegrees = fabs(degreesA - degreesB);

	if (angularDistanceDegrees > 180.f)
	{
		angularDistanceDegrees = fmod(angularDistanceDegrees, 360.f);
		
		if (angularDistanceDegrees > 180.f)
		{
			angularDistanceDegrees = 360 - angularDistanceDegrees;
		}
	}

	return angularDistanceDegrees;
}

float CalcAngularDistanceRadians(float radiansA, float radiansB)
{
	float angularDistanceRadians = fabs(radiansA - radiansB);

	while (angularDistanceRadians > PI)
	{
		angularDistanceRadians = fmod(angularDistanceRadians,( 2 * PI ));

		if (angularDistanceRadians > PI)
		{
			angularDistanceRadians = (2 * PI) - angularDistanceRadians;
		}
	}

	return angularDistanceRadians;
}

float CalcAngularDisplacementDegrees(float startDegrees, float endDegrees)
{
	float angularDisplacementDegrees = endDegrees - startDegrees;

	while( angularDisplacementDegrees > 180.f )
	{
		angularDisplacementDegrees -= 360.f;
	}
	while( angularDisplacementDegrees < -180.f )
	{
		angularDisplacementDegrees += 360.f;
	}

	return angularDisplacementDegrees;
}

float CalcAngularDisplacementRadians(float startRadians, float endRadians)
{
	float angularDisplacementRadians = endRadians - startRadians;

	while( angularDisplacementRadians > PI )
	{
		angularDisplacementRadians -= ( 2.f * PI );
	}
	while( angularDisplacementRadians < -PI )
	{
		angularDisplacementRadians += ( 2.f * PI );
	}
	
	return angularDisplacementRadians;
}

bool CheckIfIsPowerOfTwo(int a)
{
	bool isPowerOfTwo = false;
	int argumentMinusOne = a-1;

	if ( (a & argumentMinusOne) == 0x0 )
	{
		isPowerOfTwo = true;
	}

	return isPowerOfTwo;
}

float InterpolateFloat(float startFloatValue, float endFloatValue, float fractionComplete)
{
	return ((startFloatValue * (1.f - fractionComplete)) + (endFloatValue * fractionComplete));
}

int InterpolateInt(int startIntValue, int endIntValue, float fractionComplete)
{
	float floatInterpolationValue = InterpolateFloat((float)(startIntValue), float(endIntValue), fractionComplete);
	return (int)(floatInterpolationValue);
}

//Range Mapping
//
float RangeMapFloat( float currentValue, float currentStartingPoint, float currentEndPoint,
						float newStartingPoint, float newEndPoint, bool clamp,
						FilterFunctionPtr filterFunction)
{
	//we return the midpoint if the current starting point and end point are the same
	if( currentStartingPoint == currentEndPoint )
	{
		return ( newStartingPoint * .5f + newEndPoint * .5f );
	}

	float fraction = ( currentValue - currentStartingPoint ) / ( currentEndPoint - currentStartingPoint );

	//clamping the value if the user desired to
	if( clamp )
	{
		if( fraction < 0.f )
		{
			fraction = 0.f;
		}
		else if( fraction > 1.f )
		{
			fraction = 1.f;
		}
	}

	//Apply any desired transformation
	//
	if (filterFunction)
	{
		fraction = filterFunction(fraction);
	}
	
	//mapping the value into the new range
	float newValue = newStartingPoint + fraction * ( newEndPoint - newStartingPoint );
	return newValue;
}

float SmoothStartFilter( float value )
{
	// x^2
	return ( value * value );
}

float SmoothStopFilter( float value )
{
	// 1 - ( 1 - x )^2
	return ( 1 - ( ( 1 - value ) * ( 1 - value ) ) );
}

float SmoothStepFilter( float value )
{
	//( 1 - x )( x^2 ) + x[ 1 - ( 1 - x )^2 ]
	//	= 3x^2 - 2x^3
	return ( ( 3 * value * value ) - ( 2 * value * value * value ) );
}

float LinearFilter( float value )
{
	return value;
}

float SlingshotFilter( float value )
{
	float topPart = ( 1.280776406f * 1.280776406f ) * value;
	float bottomPart = ( value * value ) + ( 1.280776406f * .5f );

	return topPart/bottomPart;
}

//
bool isPointBehindFace( const Vector2& faceStart, const Vector2& faceEnd, const Vector2& pointToCheck )
{
	//calculating the normal of the edge made by the two points
	Vector2 normalVector = faceEnd - faceStart;
	normalVector.RotateMinus90Degrees();

	//checking if the point lies behind the face
	Vector2 startToTestPoint = pointToCheck - faceStart;
	
	float dotProduct = startToTestPoint.DotProduct(normalVector);
	bool isTestPointBehind = ( dotProduct < 0.f );

	return isTestPointBehind;
}

Vector2 getNormal( const Vector2& startPoint, const Vector2& endPoint )
{
	Vector2 normalVector = endPoint - startPoint;
	normalVector.RotateMinus90Degrees();

	return normalVector;
}

float dotProduct( const Vector2& vectorA, const Vector2& vectorB )
{
	return vectorA.x * vectorB.x + vectorA.y * vectorB.y;
}

float dotProduct( const Vector3& vectorA, const Vector3& vectorB )
{
	return vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z;
}

float getSign( float value )
{
	return ( value >= 0.f ? 1.f : -1.f );
}

unsigned int toRGBA( float r, float g, float b, float a /*= 1.0f */ )
{
	return	( ( static_cast< unsigned int >( r * 255.0f ) & 0xFF ) << 0  ) |
		( ( static_cast< unsigned int >( g * 255.0f ) & 0xFF ) << 8  ) |
		( ( static_cast< unsigned int >( b * 255.0f ) & 0xFF ) << 16 ) |
		( ( static_cast< unsigned int >( a * 255.0f ) & 0xFF ) << 24 );
}

void separateData( const std::string& dataString, std::vector< std::string >& dataStrings, std::vector< float >& weightsOfData )
{
	//construct the list of choices
	std::vector< std::string > choices = splitString( dataString, ',' );

	//separate the list of choices into strings with weights
	std::vector< std::string > currentChoiceData;
	std::for_each( choices.begin(), choices.end(), [&]( const std::string& currentChoiceString )
	{
		currentChoiceData = splitString( currentChoiceString, '@' );

		//if the data returns with only one parameter, then we don't add it into the pool of choices
		if( currentChoiceData.size() > 1 )
		{
			dataStrings.push_back( currentChoiceData[0] );
			weightsOfData.push_back( float( atof( currentChoiceData[1].c_str() ) ) );
		}
	});
}

std::vector< std::string > splitString( const std::string& dataString, const char& key )
{	
	size_t keyLocation = 0;
	size_t startIndex = 0;
	size_t currentIndex = 0;
	std::string substring;
	std::vector< std::string > stringContainer;

	keyLocation = dataString.find( key, currentIndex );
	while ( keyLocation != std::string::npos )
	{
		substring = dataString.substr( startIndex, keyLocation-startIndex );

		//remove whitespace
		removeFrontAndBackWhiteSpace( substring );

		//if the string is not empty, then we add it to the container of keys
		if( !substring.empty() )
		{
			stringContainer.push_back( substring );
		}

		startIndex = keyLocation+1;
		keyLocation = dataString.find( key, startIndex );
	}

	if( startIndex < dataString.size() )
	{
		substring = dataString.substr( startIndex );

		//remove whitespace
		removeFrontAndBackWhiteSpace( substring );

		//if the string is not empty, then we add it to the container of keys
		if( !substring.empty() )
		{
			stringContainer.push_back( substring );
		}
	}
	
	return stringContainer;

};

void removeWhiteSpace( std::string& line, bool front )
{
	size_t index = std::string::npos;

	if( front )
	{
		index = line.find_first_not_of( " \t\n" );
		if( index == std::string::npos )
		{
			line.clear();
		}
		else
		{
			line.erase( 0, index );
		}
	}
	else
	{
		index = line.find_last_not_of( " \t\n" );
		if( index == std::string::npos )
		{
			line.clear();
		}
		else
		{
			line = line.substr( 0, index + 1 );
		}
	}
}

void removeFrontAndBackWhiteSpace( std::string& textBlock )
{
	removeWhiteSpace( textBlock, true );
	removeWhiteSpace( textBlock, false );
}

float RandZeroToOne()
{
	return float( rand() ) / float( RAND_MAX );
}
