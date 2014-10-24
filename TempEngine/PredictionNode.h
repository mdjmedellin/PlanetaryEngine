#pragma once
#ifndef PREDICTION_NODE_H
#define PREDICTION_NODE_H

//includes
#include <memory>
#include <string>
#include <vector>
//

namespace gh
{
	class PredictionNode : public std::enable_shared_from_this< PredictionNode >
	{
	public:
		PredictionNode();
		PredictionNode( const std::string& nodeValue );
		bool addChild( const std::string& toAdd );
		bool isLessThan( const std::shared_ptr< PredictionNode >toCompareAgainst );
		bool isLessThan( const std::string& toCompareAgainst );
		std::string getValue();
		void getTopXList( std::vector< std::string >& topXList, size_t listSizeLimit, const std::string& currentString );

	private:
		std::string trimFrontWhiteSpace( const std::string& currentString );
		void getLeafs( std::vector< std::string >& leafContainer, size_t containerSizeLimit );
		void getClosestMatchList( std::vector< std::string >& topXList, size_t listSizeLimit, const std::string& toLookFor );

		std::vector< std::shared_ptr< PredictionNode > > m_children;
		std::string m_value;
	};
}

#endif