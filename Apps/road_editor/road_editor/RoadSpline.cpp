#include "StdAfx.h"
#include "RoadSpline.h"
#include "LaneNode.h"
#include "MatrixStack.h"
#include "MathUtilities.hpp"
#include "Vehicle.h"

namespace gh
{
	void RoadSpline::pushLaneNode( LaneNode* in_laneNode )
	{
		//create a new spline node out of the incoming LaneNode
		SplineNode splineNodeToAdd;
		splineNodeToAdd.m_position = in_laneNode->m_position;

		m_splineNodes.push_back( splineNodeToAdd );
	}

	void RoadSpline::createSpline()
	{
		assert( m_splineNodes.size() >= 4 );

		float tension = 0.f;
		Vector3 currentNodePosition;
		Vector3 initialVelocity;
		Vector3 finalVelocity;
		size_t numOfSplineNodes = m_splineNodes.size();

		double s = 0.0;
		double s_squared = 0.0;
		double t = 0.0;
		double t_squared = 0.0;
		Vector3 initialPosition;
		Vector3 finalPosition;
		Vector3 A_Component;
		Vector3 D_Component;
		Vector3 U_Component;
		Vector3 V_Component;
		Vector3 prevLocation;
		float currentLength = 0.f;

		//velocity of first and last node is 0
		m_splineNodes[0].m_velocity = Vector3();
		m_splineNodes[ numOfSplineNodes - 1 ].m_velocity = Vector3();

		for( size_t currentIndex = 1; currentIndex <= numOfSplineNodes - 1; ++currentIndex )
		{
			if( currentIndex == numOfSplineNodes - 1 )
			{
				finalVelocity.setXYZ( 0.f, 0.f, 0.f );
			}
			else
			{
				finalVelocity = m_splineNodes[ currentIndex + 1 ].m_position - m_splineNodes[ currentIndex - 1 ].m_position;
				finalVelocity.scaleUniform( .5f );
				finalVelocity.scaleUniform( 1.f - tension );
			}

			m_splineNodes[ currentIndex ].m_velocity = finalVelocity;

			initialPosition = m_splineNodes[ currentIndex - 1 ].m_position;
			finalPosition = m_splineNodes[ currentIndex ].m_position;
			prevLocation = initialPosition;
			currentLength = 0.f;

			for( t = 0.0; t <= 1.0; t+=.0001 )
			{
				t_squared = t * t;
				s = ( 1.0 - t );
				s_squared = s * s;

				A_Component = float( s_squared * ( 1.0 + ( 2.0 * t ) ) ) * initialPosition;
				D_Component = float( t_squared * ( 1.0 + ( 2.0 * s ) ) ) * finalPosition;
				U_Component = float( s_squared * t ) * initialVelocity;
				V_Component = float( s * t_squared ) * finalVelocity;

				currentNodePosition = A_Component + D_Component + U_Component - V_Component;

				currentLength += ( currentNodePosition - prevLocation ).calculateRadialDistance();
				prevLocation = currentNodePosition;
			}

			m_splineNodes[ currentIndex - 1 ].m_distanceToNextSplineNode = currentLength;
			m_splineNodes[ currentIndex - 1 ].m_inverseDistanceToNextSplineNode = 1.f / currentLength;
			m_totalLength += currentLength;
			initialVelocity = finalVelocity;
		}
	}

	bool RoadSpline::getPosition( float in_pointInSpline, Vector3& out_position )
	{
		if( in_pointInSpline > m_totalLength )
		{
			return false;
		}
		else
		{
			size_t index = 0;
			for( ; index < m_splineNodes.size() 
				&& in_pointInSpline > m_splineNodes[ index ].m_distanceToNextSplineNode; ++index )
			{
				in_pointInSpline -= m_splineNodes[ index ].m_distanceToNextSplineNode;
			}

			if( index >= m_splineNodes.size() - 1 )
			{
				out_position = m_splineNodes[ m_splineNodes.size() - 1 ].m_position;
			}
			else
			{
				double t = in_pointInSpline * m_splineNodes[ index ].m_inverseDistanceToNextSplineNode;
				double t_squared = t * t;
				double s = ( 1.0 - t );
				double s_squared = s * s;

				Vector3 A_Component = float( s_squared * ( 1.0 + ( 2.0 * t ) ) ) * m_splineNodes[ index ].m_position;
				Vector3 D_Component = float( t_squared * ( 1.0 + ( 2.0 * s ) ) ) * m_splineNodes[ index + 1 ].m_position;
				Vector3 U_Component = float( s_squared * t ) * m_splineNodes[ index ].m_velocity;
				Vector3 V_Component = float( s * t_squared ) * m_splineNodes[ index + 1 ].m_velocity;

				out_position = A_Component + D_Component + U_Component - V_Component;
			}

			return true;
		}
	} 

	void RoadSpline::render( MatrixStack& currentMatrixStack, float scale, const Rgba& color /* = Rgba::WHITE */ )
	{
		size_t splineNodeIndex = 0;
		double t_squared = 0.0; 
		double s = 0.0;
		double s_squared = 0.0;
		Vector3 A_Component;
		Vector3 D_Component;
		Vector3 U_Component;
		Vector3 V_Component;
		Vector3 calculatedPosition;


		glPointSize( 2.f * scale );
		glColor3f( 1.f, 1.f, 0.f );
		glBegin( GL_POINTS );
		for( splineNodeIndex = 0; splineNodeIndex < m_splineNodes.size(); ++splineNodeIndex )
		{
			calculatedPosition = m_splineNodes[ splineNodeIndex ].m_position;
			glVertex3f( calculatedPosition.x, calculatedPosition.y, calculatedPosition.z );
		}
		glEnd();

		glPointSize( 2.f );
		glColor3f( color.m_R, color.m_G, color.m_B );
		glBegin( GL_POINTS );
		for( splineNodeIndex = 0; splineNodeIndex < m_splineNodes.size() - 1; ++splineNodeIndex )
		{
			for( double t = 0.f; t <= 1.0; t += .1 )
			{
				t_squared = t * t;
				s = ( 1.0 - t );
				s_squared = s * s;

				A_Component = float( s_squared * ( 1.0 + ( 2.0 * t ) ) ) * m_splineNodes[ splineNodeIndex ].m_position;
				D_Component = float( t_squared * ( 1.0 + ( 2.0 * s ) ) ) * m_splineNodes[ splineNodeIndex + 1 ].m_position;
				U_Component = float( s_squared * t ) * m_splineNodes[ splineNodeIndex ].m_velocity;
				V_Component = float( s * t_squared ) * m_splineNodes[ splineNodeIndex + 1 ].m_velocity;

				calculatedPosition = A_Component + D_Component + U_Component - V_Component;

				glVertex3f( calculatedPosition.x, calculatedPosition.y, calculatedPosition.z );
			}
		}

		glEnd();
		glPointSize( 1.f );
	}

	float RoadSpline::getRandomDistanceWithin()
	{
		return RandZeroToOne() * m_totalLength;
	}

	RoadSpline::RoadSpline()
		:	m_totalLength( 0.f )
		,	m_splineNodes( std::vector< SplineNode >() )
		,	m_vehiclesOnSpline( std::vector< Vehicle* >() )
	{}

	float RoadSpline::getLength()
	{
		return m_totalLength;
	}

	bool RoadSpline::getDirection( float in_pointInSpline, Vector3& out_direction )
	{
		if( in_pointInSpline > m_totalLength )
		{
			return false;
		}
		else
		{
			size_t index = 0;
			for( ; index < m_splineNodes.size() 
				&& in_pointInSpline > m_splineNodes[ index ].m_distanceToNextSplineNode; ++index )
			{
				in_pointInSpline -= m_splineNodes[ index ].m_distanceToNextSplineNode;
			}

			if( index >= m_splineNodes.size() - 1 )
			{
				index = m_splineNodes.size() - 2;
				in_pointInSpline = m_splineNodes[ index ].m_distanceToNextSplineNode;
			}

			double t = in_pointInSpline * m_splineNodes[ index ].m_inverseDistanceToNextSplineNode;
			double t_squared = t * t;
			double s = ( 1.0 - t );
			double s_squared = s * s;

			//6 * A * ( t - 1 ) * t - 6 * D * ( t - 1 ) * t + 3 * t^2 * U + 3 * t^2 * V - 4 * t * U - 2 * t * V + U
			Vector3 VelocityA_Component = 6.f * m_splineNodes[ index ].m_position * float( ( t - 1 ) * t );
			Vector3 VelocityB_Component = -6.f * m_splineNodes[ index + 1 ].m_position * float( ( t - 1 ) * t );
			Vector3 VelocityU_Component = float( 3.0 * t_squared ) * m_splineNodes[ index ].m_velocity - float( 4.0 * t ) * m_splineNodes[ index ].m_velocity + m_splineNodes[ index ].m_velocity;
			Vector3 VelocityV_Component = float( 3.0 * t_squared ) * m_splineNodes[ index + 1 ].m_velocity - float( 2.0 * t ) * m_splineNodes[ index + 1 ].m_velocity;

			out_direction = VelocityA_Component + VelocityB_Component + VelocityU_Component + VelocityV_Component;
			out_direction.normalize();

			return true;
		}
	}

	Vehicle* RoadSpline::getVehicleInFrontOf( Vehicle* vehicleOnBack, float startingPoint )
	{
		float distanceTraversedByVehicle = startingPoint;

		Vehicle* closestVehicleInFront = nullptr;
		float distanceClosestVehicleIsAhead = -1.f;

		//iterate through all the vehicles that are currently stored and check for the closest one ahead
		//of this vehicle

		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehiclesOnSpline.begin();
			currentVehicle != m_vehiclesOnSpline.end(); ++currentVehicle )
		{
			if(  vehicleOnBack == ( *currentVehicle ) )
			{
				continue;
			}

			//calculate the distance this vehicle is ahead
			if( ( (*currentVehicle)->m_lengthTraveledAlongSpline > distanceTraversedByVehicle &&
				(*currentVehicle)->m_lengthTraveledAlongSpline - distanceTraversedByVehicle < distanceClosestVehicleIsAhead ) ||
				 distanceClosestVehicleIsAhead < 0.f )
			{
				distanceClosestVehicleIsAhead = (*currentVehicle)->m_lengthTraveledAlongSpline
													- distanceTraversedByVehicle;

				if( distanceClosestVehicleIsAhead > 0.f )
				{
					//this vehicle is in front of the car
					closestVehicleInFront = *currentVehicle;
				}
			}
		}

		return closestVehicleInFront;
	}

}