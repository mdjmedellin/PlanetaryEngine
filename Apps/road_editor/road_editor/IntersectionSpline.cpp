#include "StdAfx.h"
#include "IntersectionSpline.h"
#include "MathUtilities.hpp"
#include "Rgba.h"

namespace gh
{
	bool IntersectionSpline::getPosition( float in_pointInSpline, Vector3& out_position )
	{
		in_pointInSpline += m_splineNodes[0].m_distanceToNextSplineNode
			+ m_splineNodes[3].m_distanceToNextSplineNode;

		return RoadSpline::getPosition( in_pointInSpline, out_position );
	}

	bool IntersectionSpline::getDirection( float in_pointInSpline, Vector3& out_direction )
	{
		in_pointInSpline += m_splineNodes[0].m_distanceToNextSplineNode;

		return RoadSpline::getDirection( in_pointInSpline, out_direction );
	}

	float IntersectionSpline::getRandomDistanceWithin()
	{
		return RandZeroToOne() 
			* ( m_totalLength
			- m_splineNodes[0].m_distanceToNextSplineNode
			- m_splineNodes[3].m_distanceToNextSplineNode );
	}

	float IntersectionSpline::getLength()
	{
		return ( m_totalLength 
			- m_splineNodes[0].m_distanceToNextSplineNode
			- m_splineNodes[2].m_distanceToNextSplineNode );
	}

	void IntersectionSpline::render( MatrixStack& currentMatrixStack, const Rgba& color )
	{
		size_t splineNodeIndex = 0;
		float t_squared = 0.0f; 
		float s = 0.0f;
		float s_squared = 0.0f;
		Vector3 A_Component;
		Vector3 D_Component;
		Vector3 U_Component;
		Vector3 V_Component;
		Vector3 calculatedPosition;


		glPointSize( 5.f );
		glColor3f( 1.f, 1.f, 0.f );
		glBegin( GL_POINTS );
		for( splineNodeIndex = 1; splineNodeIndex < m_splineNodes.size()-1; ++splineNodeIndex )
		{
			calculatedPosition = m_splineNodes[ splineNodeIndex ].m_position;
			glVertex3f( calculatedPosition.x, calculatedPosition.y, calculatedPosition.z );
		}
		glEnd();

		glPointSize( 2.f );
		glColor3f( color.m_R, color.m_G, color.m_B );
		glBegin( GL_POINTS );
		for( splineNodeIndex = 1; splineNodeIndex < m_splineNodes.size() - 2; ++splineNodeIndex )
		{
			for( float t = 0.f; t <= 1.0f; t += .1f )
			{
				t_squared = t * t;
				s = ( 1.0f - t );
				s_squared = s * s;

				A_Component = ( s_squared * ( 1.0f + ( 2.0f * t ) ) ) * m_splineNodes[ splineNodeIndex ].m_position;
				D_Component = ( t_squared * ( 1.0f + ( 2.0f * s ) ) ) * m_splineNodes[ splineNodeIndex + 1 ].m_position;
				U_Component = ( s_squared * t ) * m_splineNodes[ splineNodeIndex ].m_velocity;
				V_Component = ( s * t_squared ) * m_splineNodes[ splineNodeIndex + 1 ].m_velocity;

				calculatedPosition = A_Component + D_Component + U_Component - V_Component;

				glVertex3f( calculatedPosition.x, calculatedPosition.y, calculatedPosition.z );
			}
		}

		glEnd();
		glPointSize( 1.f );
	}

	IntersectionSpline::IntersectionSpline()
		:	RoadSpline()
	{}

}