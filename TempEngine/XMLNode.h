#pragma once
#ifndef XMLNODE_H
#define XMLNODE_H

#include "pugixml.hpp"
#include <functional>
#include <vector>

class IntVector2;
class Vector2;

namespace gh
{
	class FloatRange;
	class IntRange;
	class Rgba;

	enum compareResult{ NOMATCH = 0, MATCH, MISSMATCH };

	struct stringVectorCompareResult
	{
		compareResult m_compareResult;
		int m_vectorMatchIndex;

		stringVectorCompareResult()
			:	m_compareResult( NOMATCH )
			,	m_vectorMatchIndex( -1 )
		{}
	};

	class XMLNode
	{
	public:
		XMLNode();
		XMLNode( const char* szPath );
		void forEachChild( std::function< void ( const XMLNode& ) > func ) const;
		void forChild( const std::string& childName, std::function< void ( const XMLNode& ) > func ) const;
		std::string getFileName() const;
		std::string getFilePath() const;
		std::string getElementName() const;
		bool hasChildren() const;
		bool hasAttributes() const;
		bool attributeIsPresent( std::string attributeName ) const;
		bool loadXMLDocument( const char* szPath );
		bool goToRoot();
		bool goToChild( std::string childName = "" );
		bool goToNextSibling( std::string siblingName = "" );
		bool goToPrevSibling( std::string siblingName = "" );

		static stringVectorCompareResult stringCompareFunction( const std::string& value1, const std::string& value2 );
		static stringVectorCompareResult compareStringToVector( const std::vector< std::string >& vectorToIterate, const std::string& stringToCompare,
																stringVectorCompareResult (*compareFunction)( const std::string&, const std::string& ) );
		static void csvStringFromVector( std::string& outputString, const std::vector< std::string >& stringVector );
		static int GetIntXMLAttribute( const XMLNode& currentNode, std::string attributeName, int defaultValue );
		static float GetFloatXMLAttribute( const XMLNode& currentNode, std::string attributeName, float defaultValue );
		static Rgba GetRgbaXMLAttribute( const XMLNode& currentNode, std::string attributeName, Rgba defaultValue );
		static Vector2 GetVector2XMLAttribute( const XMLNode& currentNode, std::string attributeName, Vector2 defaultValue );
		static std::string GetStringXMLAttribute( const XMLNode& currentNode, std::string attributeName, std::string defaultValue );
		static bool GetBoolXMLAttribute( const XMLNode& currentNode, std::string attributeName, bool defaultValue );
		static char GetCharXMLAttribute( const XMLNode& currentNode, std::string attributeName, char defaultValue );
		static IntVector2 GetIntVector2XMLAttribute( const XMLNode& currentNode, std::string attributeName, IntVector2 defaultValue );
		static FloatRange GetFloatRangeAttribute( const XMLNode& currentNode, const std::string& attributeName, const std::string& defaultValueRange );
		static IntRange GetIntRangeAttribute( const XMLNode& currentNode, const std::string& attributeName, const std::string& defaultValueRange );

		//takes in Comma Separated Value Strings
		static bool ValidateXMLAttributes( const XMLNode& currentNode, const std::string& requiredAttributes, const std::string& optionalAttributes );
		static bool ValidateXMLChildElements( const XMLNode& currentNode, const std::string& requiredChildElements, const std::string& optionalChildElements );

		//take in vector of keys
		static bool ValidateXMLAttributes( const XMLNode& currentNode, const std::vector< std::string >& requiredAttributesVector, const std::vector< std::string >& optionalAttributesVector );
		static bool ValidateXMLChildElements( const XMLNode& currentNode, const std::vector< std::string >& requiredChildrenVector, const std::vector< std::string >& optionalChildrenVector );

		//separates keyList
		static void separateKeyList( const std::string& listOfKeys, std::vector< std::string >& keyContainer );
		
		static void removeWhiteSpace( std::string& line, bool front );
		static void removeFrontAndBackWhiteSpace( std::string& textBlock );

		std::string m_filePath;

	private:
		static pugi::xml_attribute_iterator FindAttributeLocation( const pugi::xml_node& currentNode, std::string attributeName );
		static Vector2 as_Vec2( const char* vector2Values, Vector2 defaultValue );
		static IntVector2 as_IntVec2( const char* intVector2Values, IntVector2 defaultValue );
		
		pugi::xml_document m_loadedXML;
		pugi::xml_parse_result m_parseResult;
		pugi::xml_node m_currentNode;
		std::string m_fileName;
		
	};
}

#endif