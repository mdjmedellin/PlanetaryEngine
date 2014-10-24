#include "stdafx.h"
#include "RoadPath.h"
#include "DrivingLane.h"
#include "DrivingSegment.h"

namespace gh
{
	void RoadPath::addToOpenList( DrivingSegment* in_drivingLane )
	{
		if( m_endLaneIsOnClosedList )
		{
			return;
		}

		RoadPathNode* nodeToInsertIntoOpenList = new RoadPathNode();
		nodeToInsertIntoOpenList->m_lane = in_drivingLane;

		//assign parent and gCost
		if( m_closedList.empty() )
		{
			nodeToInsertIntoOpenList->m_parentNode = nullptr;
			nodeToInsertIntoOpenList->gCost = getManhattanDistance( in_drivingLane->getStartingLocation(),
																	m_pathStartLocation );
		}
		else
		{
			nodeToInsertIntoOpenList->m_parentNode = m_closedList.back();
			nodeToInsertIntoOpenList->gCost = nodeToInsertIntoOpenList->m_parentNode->gCost +
				in_drivingLane->getLength();
		}

		//calculate h and f cost
		nodeToInsertIntoOpenList->hCost = getManhattanDistance( in_drivingLane->getEndLocation(), 
											m_endLane->getStartingLocation() );
		nodeToInsertIntoOpenList->fCost = nodeToInsertIntoOpenList->gCost + nodeToInsertIntoOpenList->hCost;

		//add to open list
		RoadPathNode* resultFromOpenListSearch = nullptr;
		resultFromOpenListSearch = findNodeInOpenList( in_drivingLane );

		if( resultFromOpenListSearch )
		{
			if( resultFromOpenListSearch->gCost > nodeToInsertIntoOpenList->gCost )
			{
				resultFromOpenListSearch->m_parentNode = m_closedList.back();
			}

			return;
		}
		else
		{
			if( in_drivingLane == m_endLane )
			{
				m_closedList.push_back( nodeToInsertIntoOpenList );
				m_endLaneIsOnClosedList = true;
			}
			else
			{
				m_openList.push_back(nodeToInsertIntoOpenList);
			}
			return;
		}
	}

	RoadPathNode* RoadPath::findNodeInOpenList( DrivingSegment* laneToFind )
	{
		for( auto iter = m_openList.begin(); iter != m_openList.end(); ++iter )
		{
			if ( (*iter)->m_lane == laneToFind )
			{
				return (*iter);
			}
		}

		return nullptr;
	}

	RoadPath::RoadPath( const DrivingSegment* in_startLane, const DrivingSegment* in_endLane )
		:	m_pathDirections( std::vector< RoadPathNode* >() )
		,	m_closedList( std::vector< RoadPathNode* >() )
		,	m_openList( std::vector< RoadPathNode* >() )
		,	m_pathStartLocation( Vector3() )
		,	m_endLane( in_endLane )
		,	m_endLaneIsOnClosedList( false )
		,	m_currentNode( nullptr )
	{
		m_pathStartLocation = in_startLane->getStartingLocation();
	}

	DrivingSegment* RoadPath::getBestOpenNode()
	{
		if( m_endLaneIsOnClosedList )
		{
			m_currentNode = nullptr;
			return nullptr;
		}

		if( !m_openList.empty() )
		{
			std::vector< RoadPathNode* >::iterator bestNode = m_openList.begin();

			for( auto currentPathNode = m_openList.begin() + 1; currentPathNode != m_openList.end(); ++currentPathNode )
			{
				if( ( *currentPathNode )->fCost < ( *bestNode )->fCost )
				{
					bestNode = currentPathNode;
				}
				else if( (*currentPathNode)->fCost == (*bestNode)->fCost )
				{
					if( (*currentPathNode)->hCost <= (*bestNode)->hCost )
					{
						bestNode = currentPathNode;
					}
				}
			}

			m_currentNode = ( *bestNode );
			m_closedList.push_back( ( *bestNode ) );
			m_openList.erase( bestNode );
				
			return m_closedList.back()->m_lane;
		}
		else
		{
			m_currentNode = nullptr;
			return nullptr;
		}
	}

	PATH_STATUS RoadPath::getPathBuildingStatus()
	{
		if( m_openList.empty() && m_currentNode == nullptr )
		{
			constructPath();
			return NO_MORE_OPEN_NODES;
		}
		else if( m_endLaneIsOnClosedList )
		{
			//construct the path
			constructPath();
			return FOUND_COMPLETE_PATH;
		}
		else
		{
			return BUILDING_GOOD;
		}
	}

	void RoadPath::constructPath()
	{
		//THIS NEEDS MORE DEBUGGING
		m_pathDirections.clear();

		if( !m_closedList.empty() )
		{
			RoadPathNode* currentNode = m_closedList.back();
	
			while( currentNode )
			{
				m_pathDirections.push_back( currentNode );
				currentNode = currentNode->m_parentNode;
			}
		}
	}

	DrivingSegment* RoadPath::getNextLaneInPath()
	{
		DrivingSegment* nextLaneInPath = m_pathDirections.back()->m_lane;
		m_pathDirections.pop_back();

		return nextLaneInPath;
	}

	DrivingSegment* RoadPath::peekNextLaneInPath()
	{
		if( !m_pathDirections.empty() )
		{
			return m_pathDirections.back()->m_lane;
		}
		else
		{
			return nullptr;
		}
	}

}