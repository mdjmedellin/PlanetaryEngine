#pragma once
#ifndef INT_RANGE_H
#define INT_RANGE_H

//includes
#include "Vector2.hpp"
#include <string>
//

namespace gh
{
	class IntRange
	{
		Vector2 m_rangeValues;		// x = m_minValue		y = m_maxValue

		void ParseStringValues( std::string& rangeTextString );
		Vector2 asIntRange( const char* intRangeString );


	public:
		IntRange();
		IntRange( const std::string& asText );
		int GetRandomValueInRange() const;
	};
}

#endif