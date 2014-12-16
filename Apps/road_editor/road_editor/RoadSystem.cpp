#include "stdafx.h"
#include "RoadSystem.h"
#include "Road.h"
#include "MatrixStack.h"
#include "MathUtilities.hpp"
#include "Vector3.hpp"
#include "DrivingLane.h"
#include "Intersection.h"
#include "roadPath.h"

namespace gh
{
	RoadSystem::RoadSystem()
		:	m_roads( std::vector< Road* >() )
	{}

	void RoadSystem::render( MatrixStack& currentMatrixStack )
	{
		//render all the roads
		for( std::vector< Road* >::iterator currentRoad = m_roads.begin(); currentRoad != m_roads.end(); ++currentRoad )
		{
			(*currentRoad)->render( currentMatrixStack );
		}
		//render all the intersections
		for( std::vector< Intersection* >::iterator currentIntersection = m_intersections.begin();
			currentIntersection != m_intersections.end(); ++currentIntersection )
		{
			(*currentIntersection)->render( currentMatrixStack );
		}
	}

	DrivingLane* RoadSystem::getRandomLane()
	{
		if( !m_roads.empty() )
		{
			//choose a random road
			int road = int( RoundFloatToNearestInt( RandZeroToOne() * float( m_roads.size() - 1 ) ) );
			return m_roads[ road ]->getRandomDrivingLane();
		}
		else
		{
			return nullptr;
		}
	}

	void RoadSystem::addRoad( Road* roadAdded )
	{
		m_roads.push_back( roadAdded );

		//extract all driving lanes from the road
		std::vector< DrivingLane* > thisRoadsDrivingLanes;
		roadAdded->getAllDrivingLanes( thisRoadsDrivingLanes );

		//add the driving lanes to the map
		for( std::vector< DrivingLane* >::iterator currentDrivingLane = thisRoadsDrivingLanes.begin();
			currentDrivingLane != thisRoadsDrivingLanes.end(); ++currentDrivingLane )
		{
			m_drivingLanes[ (*currentDrivingLane)->getID() ] = (*currentDrivingLane);
		}
	}

	RoadPath* RoadSystem::calculatePathTo( int drivingLaneID, float splineLocation, DrivingSegment* startingDrivingLane )
	{
		//find the lane we are trying to get to
		std::map< int, DrivingLane* >::iterator endLaneMappedValue = m_drivingLanes.find( drivingLaneID );

		if( endLaneMappedValue != m_drivingLanes.end() )
		{
			DrivingSegment* endDrivingLane = endLaneMappedValue->second;
			RoadPath* vehiclePath = new RoadPath( startingDrivingLane, endDrivingLane );
			
			DrivingSegment* currentDrivingLane = startingDrivingLane;
			Intersection* currentIntersection = nullptr;

			do 
			{
				currentIntersection = currentDrivingLane->getEndIntersection();

				if( currentIntersection )
				{
					//get all the connecting lanes
					std::vector< DrivingSegment* > connectingLanes;
					currentIntersection->getConnectingLanes( connectingLanes, currentDrivingLane );

					//add the connecting lanes into the open list
					for( std::vector< DrivingSegment* >::iterator currentConnectingLane = connectingLanes.begin();
						currentConnectingLane != connectingLanes.end(); ++currentConnectingLane )
					{
						//here we add the path to the open list with the costs
						vehiclePath->addToOpenList( ( *currentConnectingLane ) );
					}
				}

				currentDrivingLane = vehiclePath->getBestOpenNode();

			} while ( vehiclePath->getPathBuildingStatus() == BUILDING_GOOD );

			return vehiclePath;
		}
		else
		{
			return nullptr;
		}
	}

	void RoadSystem::addIntersection( Intersection* intersection )
	{
		//go through all the intersections and add the outgoing roads
		m_intersections.push_back( intersection );
	}

	void RoadSystem::update( double deltaTime )
	{
		//update all intersection lights
		for( auto currentIntersection = m_intersections.begin(); currentIntersection != m_intersections.end(); ++currentIntersection )
		{
			(*currentIntersection)->update(deltaTime);
		}

		for( std::vector< Road* >::iterator currentRoad = m_roads.begin();
			currentRoad != m_roads.end(); ++currentRoad )
		{
			(*currentRoad)->update( deltaTime );
		}
	}

	RoadSystem::~RoadSystem()
	{
		//delete all the roads
		for( std::vector< Road* >::iterator currentRoad = m_roads.begin(); currentRoad != m_roads.end();
			++currentRoad )
		{
			delete ( *currentRoad );
		}
		m_roads.clear();
	}

	DrivingLane* RoadSystem::GetLaneWithSpecifiedIndex( int drivingLaneIndex )
	{
		std::map< int, DrivingLane* >::iterator laneMappedValue = m_drivingLanes.find( drivingLaneIndex );

		if( laneMappedValue != m_drivingLanes.end() )
		{
			return laneMappedValue->second;
		}
		else
		{
			return nullptr;
		}
	}

}