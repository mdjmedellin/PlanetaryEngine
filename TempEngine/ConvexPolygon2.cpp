#include "ConvexPolygon2.hpp"
#include "Vector2.hpp"
#include "MathUtilities.hpp"
#include <vector>

ConvexPolygon2::ConvexPolygon2()
{
	m_Points.clear();
}

ConvexPolygon2::ConvexPolygon2( const std::vector<Vector2>& points )
{
	for( auto iter = points.begin(); iter != points.end(); ++iter )
	{
		addPoint(*iter);
	}
}

ConvexPolygon2::ConvexPolygon2( const ConvexPolygon2& toCopy )
{
	m_Points = toCopy.m_Points;
}

ConvexPolygon2::~ConvexPolygon2()
{}

void ConvexPolygon2::addPoints( const std::vector< Vector2 >& pointsToAdd )
{
	for( auto iter = pointsToAdd.begin(); iter != pointsToAdd.end(); ++iter )
	{
		addPoint( *iter );
	}
}

std::vector< Vector2 > ConvexPolygon2::getPoints() const
{
	return m_Points;
}

void ConvexPolygon2::addPoint( const Vector2& newPoint )
{
	if( m_Points.empty() || m_Points.size() < 2 )
	{
		m_Points.push_back( newPoint );
		return;
	}

	std::vector< Vector2 > oldPoints = m_Points;
	m_Points.clear();

	unsigned long numOldVerts = oldPoints.size();
	bool isNewPointBehindPreviousFace = isPointBehindFace( oldPoints[ numOldVerts - 1 ], oldPoints[ 0 ], newPoint );

	for( unsigned int index = 0; index < numOldVerts; ++index )
	{
		int nextIndex = ( index + 1 < numOldVerts ) ? index + 1 : 0;
		bool isNewPointBehindNextFace = isPointBehindFace( oldPoints[index], oldPoints[nextIndex], newPoint );

		if( isNewPointBehindNextFace )
		{
			m_Points.push_back( oldPoints[ index ] );
		}
		else if( isNewPointBehindPreviousFace )
		{
			m_Points.push_back( oldPoints[ index ] );
			m_Points.push_back( newPoint );
		}

		isNewPointBehindPreviousFace = isNewPointBehindNextFace;
	}
}