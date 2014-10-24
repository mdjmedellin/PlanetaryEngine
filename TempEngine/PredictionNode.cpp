
#include "PredictionNode.h"

namespace gh
{
	bool PredictionNode::addChild( const std::string& toAdd )
	{
		if( m_value.length() < toAdd.length() )
		{

		if( m_children.empty() )
		{
			auto pNodeToAdd = std::make_shared< PredictionNode >( toAdd.substr( 0, m_value.length() + 1 ) );
			m_children.push_back( pNodeToAdd );
			
			if( m_value.length() + 1 < toAdd.length() )
			{
				return pNodeToAdd->addChild( toAdd );
			}
			else
			{
				return true;
			}
		}
		else
		{
			size_t offset = 0;
			for( auto iter = m_children.begin(); ( iter + offset ) != m_children.end(); ++offset )
			{
				if( !( *( iter + offset ) )->isLessThan( toAdd.substr( 0, m_value.length() + 1 ) ) )
				{
					break;
				}
			}
			if( offset == m_children.size() )
			{
				auto pNodeToAdd = std::make_shared< PredictionNode >( toAdd.substr( 0, m_value.length() + 1 ) );
				m_children.push_back( pNodeToAdd );

				if( m_value.length() + 1 == toAdd.length() )
				{
					return true;
				}
				else
				{
					return pNodeToAdd->addChild( toAdd );
				}
			}
			else
			{
			//check if the value of the current position is our word
			if( !m_children.empty() && (*( m_children.begin() + offset ))->getValue() == toAdd.substr( 0, m_value.length() + 1 ) )
			{
				if( m_value.length() + 1 == toAdd.length() )
				{
					return true;
				}
				else
				{
					return (*( m_children.begin() + offset ))->addChild( toAdd );
				}
			}
			else
			{
				auto pNodeToAdd = std::make_shared< PredictionNode >( toAdd.substr( 0, m_value.length() + 1 ) );
				m_children.insert( m_children.begin() + offset, pNodeToAdd );

				if( m_value.length() + 1 == toAdd.length() )
				{
					return true;
				}
				else
				{
					return pNodeToAdd->addChild( toAdd );
				}
			}
			}
		}
		}
		else
		{
			return false;
		}
	}

	bool PredictionNode::isLessThan( const std::shared_ptr< PredictionNode >toCompareAgainst )
	{
		return m_value < toCompareAgainst->getValue();
	}

	bool PredictionNode::isLessThan( const std::string& toCompareAgainst )
	{
		return m_value < toCompareAgainst;
	}

	std::string PredictionNode::trimFrontWhiteSpace( const std::string& currentString )
	{
		size_t nonSpaceIndex = currentString.find_first_not_of( " \t\r\n" );

		if( nonSpaceIndex == std::string::npos )
		{
			return "";
		}
		else
		{
			return currentString.substr( nonSpaceIndex );
		}
	}

	void PredictionNode::getTopXList( std::vector< std::string >& topXList, size_t listSizeLimit, const std::string& currentString )
	{
		std::string toLookFor = trimFrontWhiteSpace( currentString );

		if( toLookFor.empty() || toLookFor.find_first_of( " \n\t\r" ) != std::string::npos )
		{
			return;
		}

		for( auto iter = m_children.begin(); iter != m_children.end() && topXList.size() != listSizeLimit; ++iter )
		{
			(*iter)->getClosestMatchList( topXList, listSizeLimit, toLookFor );
		}
	}

	void PredictionNode::getClosestMatchList( std::vector< std::string >& topXList, size_t listSizeLimit, const std::string& toLookFor )
	{
		if( m_value == toLookFor.substr( 0, m_value.length() ) )
		{
			if( m_value.length() == toLookFor.length() )
			{
				getLeafs( topXList, listSizeLimit );
			}
			else
			{
				for( auto iter = m_children.begin(); iter != m_children.end() && topXList.size() != listSizeLimit; ++iter )
				{
					(*iter)->getClosestMatchList( topXList, listSizeLimit, toLookFor );
				}
			}
		}
	}

	void PredictionNode::getLeafs( std::vector< std::string >& leafContainer, size_t containerSizeLimit )
	{
		if( leafContainer.size() < containerSizeLimit )
		{
			if( m_children.empty() )
			{
				leafContainer.push_back( m_value );
			}
			else
			{
				for( auto iter = m_children.begin(); iter != m_children.end() && leafContainer.size() != containerSizeLimit; ++iter )
				{
					(*iter)->getLeafs( leafContainer, containerSizeLimit );
				}
			}
		}
	}

	std::string PredictionNode::getValue()
	{
		return m_value;
	}

	PredictionNode::PredictionNode()
		:	m_value("")
	{}

	PredictionNode::PredictionNode( const std::string& nodeValue )
		:	m_value( nodeValue )
	{}
}