#include "LineSegment3.hpp"


LineSegment3::LineSegment3()
	:	startPoint( Vector3() )
	,	endPoint( Vector3() )
{}

LineSegment3::LineSegment3( const Vector3& startPoint, const Vector3& endPoint )
	:	startPoint( startPoint )
	,	endPoint( endPoint )
{}

LineSegment3::LineSegment3( const LineSegment3& toCopy )
	:	startPoint( toCopy.startPoint )
	,	endPoint( toCopy.endPoint )
{}

LineSegment3::~LineSegment3()
{}