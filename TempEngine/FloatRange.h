#pragma once
#ifndef FLOAT_RANGE_H
#define FLOAT_RANGE_H

//includes
#include "Vector2.hpp"
#include <string>
//

namespace gh
{
	class FloatRange
	{
		Vector2 m_rangeValues;		// x = m_minValue		y = m_maxValue

		void ParseStringValues( std::string& rangeTextString );
		Vector2 asFloatRange( const char* floatRangeString );

	public:
		FloatRange();
		FloatRange( const std::string& asText );
		float GetRandomValueInRange() const;
		void operator+=( const float rangeChange );
		void operator-=( const float rangeChange );
	};
}

#endif