#include "stdafx.h"
#include "LaneNode.h"

namespace gh
{
	LaneNode::LaneNode()
	{}

	void LaneNode::addInputNode( LaneNode* inputNode )
	{
		m_inputNodes.push_back( inputNode );
	}

	void LaneNode::addOutputNode( LaneNode* outputNode )
	{
		m_outputNodes.push_back( outputNode );
	}

	LaneNode* LaneNode::getNextNode()
	{
		if( !m_outputNodes.empty() )
		{
			return (*m_outputNodes.begin());
		}
		else
		{
			return nullptr;
		}
	}

	void LaneNode::setDistance( LaneNode* outputNode, float totalLength )
	{
		m_outputNodesMap[ outputNode ] = totalLength;
	}

}