#pragma once
#ifndef LANE_NODE_H
#define LANE_NODE_H

//includes
#include <vector>
#include <map>
#include "Vector3.hpp"
//

namespace gh
{
	class LaneNode
	{
	public:
		Vector3 m_position;
		LaneNode();
		void addInputNode( LaneNode* inputNode );
		void addOutputNode( LaneNode* outputNode );
		LaneNode* getNextNode();
		void setDistance( LaneNode* outputNode, float totalLength );

	private:
		//right now they are all in the same level but we still use 3d vectors for position
		std::vector< LaneNode* > m_inputNodes;
		std::vector< LaneNode* > m_outputNodes;
		std::map< LaneNode*, float > m_outputNodesMap;
	};
}

#endif