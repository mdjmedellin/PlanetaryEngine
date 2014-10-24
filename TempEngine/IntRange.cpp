#include "IntRange.h"
#include "MathUtilities.hpp"
#include <time.h>

namespace gh
{
	IntRange::IntRange( const std::string& asText )
	{
		srand ( time(NULL) );
		std::string rangeTextString = asText;
		ParseStringValues( rangeTextString );
	}

	IntRange::IntRange()
		:	m_rangeValues( Vector2() )
	{}

	void IntRange::ParseStringValues( std::string& rangeTextString )
	{
		//remove front and back space
		//XMLNode::removeFrontAndBackWhiteSpace( rangeTextString );

		m_rangeValues = asIntRange( rangeTextString.c_str() );
	}

	Vector2 IntRange::asIntRange( const char* intRangeString )
	{
		bool prevEmpty = true;
		size_t startIndex = 0;

		char* subbuff = NULL;

		size_t vectorIndex = 0;

		Vector2 toReturn;

		bool keepReading = true;
		bool returnDefault = false;

		for( size_t index = 0; keepReading && !returnDefault; ++index )
		{
			switch( *( intRangeString + index ) )
			{

				//what happens when we detect a space
			case ' ':
				{
					//if we previously did not encounter an empty space, then we retrieve the previous data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc (index - startIndex + 1);
						memcpy( subbuff, &intRangeString[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = float( atof( subbuff ) );
							toReturn.y = toReturn.x;
						}
						else
						{
							toReturn.y = float( atof( subbuff ) );
							keepReading = false;
						}

						++vectorIndex;

						prevEmpty = true;
						free( subbuff );
					}
					else
					{
						startIndex = index;
					}
				}
				break;

				//what happens when we hit the null terminated character
			case 0:
				{
					//if before the null terminated character, we did not encountered an empty space, then we retrieve the data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc (index - startIndex + 1);
						memcpy( subbuff, &intRangeString[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = float( atof( subbuff ) );
							toReturn.y = toReturn.x;
						}
						else
						{
							toReturn.y = float( atof( subbuff ) );
							keepReading = false;
						}

						++vectorIndex;

						prevEmpty = true;

						keepReading = false;
						free( subbuff );
					}
				}
				break;

			case'~':
				{
					//if before the null terminated character, we did not encountered an empty space, then we retrieve the data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc (index - startIndex + 1);
						memcpy( subbuff, &intRangeString[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = float( atof( subbuff ) );
							toReturn.y = toReturn.x;
						}
						else
						{
							toReturn.y = float( atof( subbuff ) );
							keepReading = false;
						}

						++vectorIndex;

						prevEmpty = true;

						free( subbuff );
					}
					else
					{
						startIndex = index;
					}
				}
				break;

				//case where we encounter data that is part of the vector pair value
			default:
				{
					//we only move the start index when we encounter data after an empty space.
					if( prevEmpty )
					{
						startIndex = index;
						prevEmpty = false;
					}
				}
			}
		}

		//make the numbers integers
		toReturn.x = float( GetNearestInt( toReturn.x ) );
		toReturn.y = float( GetNearestInt( toReturn.y ) );

		return toReturn;
	}

	int IntRange::GetRandomValueInRange() const
	{
		float randomNumber = RandZeroToOne();

		float returnValue = m_rangeValues.x + ( randomNumber * ( m_rangeValues.y - m_rangeValues.x ) );
		return GetNearestInt( returnValue );
	}

}