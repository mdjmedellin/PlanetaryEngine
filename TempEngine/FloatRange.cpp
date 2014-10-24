#include "FloatRange.h"
#include <time.h>

namespace gh
{
	FloatRange::FloatRange( const std::string& asText )
		:	m_rangeValues( 0.f, 0.f )
	{
		srand ( time(NULL) );
		std::string rangeTextString = asText;
		ParseStringValues( rangeTextString );
	}

	FloatRange::FloatRange()
		:	m_rangeValues( Vector2() )
	{}

	void FloatRange::ParseStringValues( std::string& rangeTextString )
	{
		//remove front and back space
		//XMLNode::removeFrontAndBackWhiteSpace( rangeTextString );

		m_rangeValues = asFloatRange( rangeTextString.c_str() );
	}

	Vector2 FloatRange::asFloatRange( const char* floatRangeString )
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
			switch( *( floatRangeString + index ) )
			{

				//what happens when we detect a space
			case ' ':
				{
					//if we previously did not encounter an empty space, then we retrieve the previous data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc (index - startIndex + 1);
						memcpy( subbuff, &floatRangeString[ startIndex ], ( index - startIndex ) );
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
						memcpy( subbuff, &floatRangeString[ startIndex ], ( index - startIndex ) );
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
					if( !prevEmpty )
					{
						subbuff = (char*) malloc (index - startIndex + 1);
						memcpy( subbuff, &floatRangeString[ startIndex ], ( index - startIndex ) );
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

		return toReturn;
	}

	float FloatRange::GetRandomValueInRange() const
	{
		float randomNumber = float( double( rand() ) / double( RAND_MAX ) );

		return m_rangeValues.x + ( randomNumber * ( m_rangeValues.y - m_rangeValues.x ) );
	}

	void FloatRange::operator+=( const float rangeChange )
	{
		m_rangeValues += Vector2( rangeChange, rangeChange );
	}

	void FloatRange::operator-=( const float rangeChange )
	{
		m_rangeValues -= Vector2( rangeChange, rangeChange );
	}

}