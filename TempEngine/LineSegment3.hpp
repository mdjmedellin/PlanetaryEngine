#pragma once
#ifndef LINESEGMENT3_H
#define LINESEGMENT3_H

#include "Vector3.hpp"

class LineSegment3
{
public:
	Vector3 startPoint, endPoint;

	LineSegment3();
	explicit LineSegment3( const Vector3& startPoint, const Vector3& endPoint );
	LineSegment3( const LineSegment3& toCopy );
	~LineSegment3();
};

#endif