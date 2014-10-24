#include "XMLNode.h"
#include <direct.h>
#include "MathUtilities.hpp"
#include <algorithm>
#include <functional>
#include "ErrorReporter.h"
#include "Rgba.h"
#include "IntVector2.h"
#include "FloatRange.h"
#include "IntRange.h"
#include "FileManager.h"

namespace gh
{
	XMLNode::XMLNode()
	{
		m_loadedXML.reset();
		m_parseResult = pugi::xml_parse_result();
		m_currentNode = pugi::xml_node();
		m_fileName = "";
		m_filePath = "";
	}

	XMLNode::XMLNode( const char* szPath )
	{
		m_fileName = std::string( szPath );

		if( !loadXMLDocument( szPath ) )
		{
			// if failed to load file display error message
			std::string errorMessage;
			errorMessage = errorMessage + "XML File Loading Error\n";
			errorMessage = errorMessage + "Failure To Load File: " + szPath + "\n\n";
			errorMessage += m_parseResult.description();
			errorMessage += "\n\n";
			errorMessage = errorMessage + "Application Will Now Close\n";

			//display on visual studio output window
			std::string vsConsoleMessage = m_filePath;
			vsConsoleMessage = vsConsoleMessage + "\n";
			vsConsoleMessage = "> File Read Error:\n> " + vsConsoleMessage;
			vsConsoleMessage = vsConsoleMessage + "> Failure to load file: " + szPath + "\n";

			ErrorReporter::throwError( "Failure To Load File", errorMessage, vsConsoleMessage );
		}
	}

	bool XMLNode::loadXMLDocument( const char* szPath )
	{
		m_loadedXML.reset();

		//load the file's contents into a buffer
		char* fileBuffer = nullptr;
		int bufferSize;
		FileManager::getSingleton().loadFile( szPath, fileBuffer, bufferSize );

		//read in the xml file and parse it in order to get the font definition
		m_parseResult = m_loadedXML.load( fileBuffer );

		if( fileBuffer )
		{
			delete[] fileBuffer;
			fileBuffer = nullptr;
		}

		//m_parseResult = m_loadedXML.load_file( szPath );
		m_currentNode = m_loadedXML.root().first_child();
		m_fileName = std::string(szPath);

		//construct and store the filepath
		char* buffer = NULL;
		if( ( buffer = _getcwd( NULL, 0 ) ) == NULL )
			perror( "_getcwd error" );
		else
		{
			m_filePath = buffer;
			m_filePath.append( "\\" + m_fileName );
			free(buffer);
		}

		if( m_parseResult.status == pugi::status_ok )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	pugi::xml_attribute_iterator XMLNode::FindAttributeLocation( const pugi::xml_node& currentNode, std::string attributeName )
	{
		pugi::xml_attribute_iterator attributeLocation = currentNode.attributes_begin();

		for( ; attributeLocation != currentNode.attributes_end(); ++attributeLocation )
		{
			std::string name = attributeLocation->name();
			if( attributeLocation->name() == attributeName )
			{
				break;
			}
		}

		return attributeLocation;
	}

	int XMLNode::GetIntXMLAttribute( const XMLNode& currentNode, std::string attributeName, int defaultValue )
	{	
		//find the location of the attribute we are to retrieve
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		//if we have a valid location for the attribute, then we retrieve it
		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return attributeLocation->as_int( defaultValue );
		}
		else
		{
			return defaultValue;
		}
	}

	float XMLNode::GetFloatXMLAttribute( const XMLNode& currentNode, std::string attributeName, float defaultValue )
	{
		//find the location of the attribute we are to retrieve
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		//if we have a valid location for the attribute, then we retrieve it
		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return attributeLocation->as_float( defaultValue );
		}
		else
		{
			return defaultValue;
		}
	}

	Vector2 XMLNode::GetVector2XMLAttribute( const XMLNode& currentNode, std::string attributeName, Vector2 defaultValue )
	{
		//find the location of the attribute we are to retrieve
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		//if we have a valid location for the attribute, then we retrieve it
		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return as_Vec2( attributeLocation->as_string(), defaultValue );
		}
		else
		{
			return defaultValue;
		}
	}

	std::string XMLNode::GetStringXMLAttribute( const XMLNode& currentNode, std::string attributeName, std::string defaultValue )
	{
		//find the location of the attribute we are to retrieve
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		//if we have a valid location for the attribute, then we retrieve it
		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return attributeLocation->as_string( defaultValue.c_str() );
		}
		else
		{
			return defaultValue;
		}
	}

	Vector2 XMLNode::as_Vec2( const char* vector2Values, Vector2 defaultValue )
	{
		bool prevEmpty = true;
		size_t startIndex = 0;

		char* subbuff = NULL;

		size_t vectorIndex = 0;

		Vector2 toReturn;

		bool keepReading = true;
		bool returnDefault = false;
		
		for( size_t index = 0; keepReading && !returnDefault; ++index )
		{
			switch( *( vector2Values + index ) )
			{

			//what happens when we detect a space
			case ' ':
				{
					//if we previously did not encounter an empty space, then we retrieve the previous data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &vector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = float( atof( subbuff ) );
						}
						else
						{
							toReturn.y = float( atof( subbuff ) );
						}

						++vectorIndex;
						free(subbuff);

						prevEmpty = true;
					}
				}
				break;

			//what happens when we hit the null terminated character
			case 0:
				{
					//if before the null terminated character, we did not encountered an empty space, then we retrieve the data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &vector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = float( atof( subbuff ) );
						}
						else
						{
							toReturn.y = float( atof( subbuff ) );
						}

						++vectorIndex;
						free( subbuff );

						prevEmpty = true;

						keepReading = false;
					}
				}
				break;

			//forward-slash is used to separate values
			case',':
				{
					//this basically does the same thing as the case when we encounter a space
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &vector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = (float)atof( subbuff );
						}
						else
						{
							toReturn.y = (float)atof( subbuff );
						}

						++vectorIndex;

						free( subbuff );

						prevEmpty = true;
					}
				}
				break;

			//case where we encounter data that is part of the vector pair value
			default:
				{
					//we only move the start index when we encounter data after an empty space.
					if( prevEmpty )
					{
						startIndex = index;
						prevEmpty = false;
					}
				}
			}

		}

		if( vectorIndex == 2 )
		{
			return toReturn;
		}
		else
		{
			return defaultValue;
		}

	}

	IntVector2 XMLNode::as_IntVec2( const char* intVector2Values, IntVector2 defaultValue )
	{
		bool prevEmpty = true;
		size_t startIndex = 0;

		char* subbuff = NULL;

		size_t vectorIndex = 0;

		IntVector2 toReturn;

		bool keepReading = true;
		bool returnDefault = false;

		for( size_t index = 0; keepReading && !returnDefault; ++index )
		{
			switch( *( intVector2Values + index ) )
			{

				//what happens when we detect a space
			case ' ':
				{
					//if we previously did not encounter an empty space, then we retrieve the previous data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &intVector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = atoi( subbuff );
						}
						else
						{
							toReturn.y = atoi( subbuff );
						}

						++vectorIndex;

						free( subbuff );

						prevEmpty = true;
					}
				}
				break;

				//what happens when we hit the null terminated character
			case 0:
				{
					//if before the null terminated character, we did not encountered an empty space, then we retrieve the data
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &intVector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = atoi( subbuff );
						}
						else
						{
							toReturn.y = atoi( subbuff );
						}

						++vectorIndex;

						free( subbuff );

						prevEmpty = true;

						keepReading = false;
					}
				}
				break;

				//forward-slash is used to separate values
			case',':
				{
					//this basically does the same thing as the case when we encounter a space
					if( !prevEmpty )
					{
						subbuff = (char*) malloc ( index - startIndex + 1 );
						memcpy( subbuff, &intVector2Values[ startIndex ], ( index - startIndex ) );
						subbuff[ index - startIndex ] = 0;

						if( vectorIndex == 0 )
						{
							toReturn.x = atoi( subbuff );
						}
						else
						{
							toReturn.y = atoi( subbuff );
						}

						++vectorIndex;

						free( subbuff );

						prevEmpty = true;
					}
				}
				break;

				//case where we encounter data that is part of the vector pair value
			default:
				{
					//we only move the start index when we encounter data after an empty space.
					if( prevEmpty )
					{
						startIndex = index;
						prevEmpty = false;
					}
				}
			}

		}

		if( vectorIndex == 2 )
		{
			return toReturn;
		}
		else
		{
			return defaultValue;
		}

	}

	bool XMLNode::goToChild( std::string childName )
	{
		if( !m_currentNode.empty() )
		{
			//if childName is empty, then we go to the first child
			if( childName.empty() )
			{
				m_currentNode = m_currentNode.first_child();
			}
			else
			{
				m_currentNode = m_currentNode.child( childName.c_str() );
			}
		}

		return !m_currentNode.empty();
	}

	bool XMLNode::goToNextSibling( std::string siblingName )
	{
		if( !m_currentNode.empty() )
		{
			//if siblingName is empty, then we go to the next sibling
			if( siblingName.empty() )
			{
				m_currentNode = m_currentNode.next_sibling();
			}
			else
			{
				m_currentNode = m_currentNode.next_sibling( siblingName.c_str() );
			}
		}

		return !m_currentNode.empty();
	}

	bool XMLNode::goToPrevSibling( std::string siblingName )
	{
		if( !m_currentNode.empty() )
		{
			//if sibling name is empty, then we go to the previous sibling
			if( siblingName.empty() )
			{
				m_currentNode = m_currentNode.previous_sibling();
			}
			else
			{
				m_currentNode = m_currentNode.previous_sibling( siblingName.c_str() );
			}
		}

		return !m_currentNode.empty();
	}

	bool XMLNode::goToRoot()
	{
		if( !m_loadedXML.empty() )
		{
			m_currentNode = m_loadedXML.root().first_child();
			return true;
		}
		else
		{
			m_currentNode = pugi::xml_node();
			return false;
		}
	}

	bool XMLNode::ValidateXMLAttributes( const XMLNode& currentNode, const std::string& requiredAttributes, const std::string& optionalAttributes )
	{
		std::vector< std::string > requiredAttributesVector;
		std::vector< std::string > optionalAttributesVector;
		
		separateKeyList( optionalAttributes, optionalAttributesVector );
		separateKeyList( requiredAttributes, requiredAttributesVector );

		return ValidateXMLAttributes( currentNode, requiredAttributesVector, optionalAttributesVector );
	}

	void XMLNode::csvStringFromVector( std::string& outputString, const std::vector< std::string >& stringVector )
	{
		for( auto iter = stringVector.begin(); iter != stringVector.end(); )
		{
			outputString += *iter;
			++iter;
			if( iter != stringVector.end() )
			{
				outputString += ",";
			}
		}
	}

	bool XMLNode::ValidateXMLAttributes( const XMLNode& currentNode, const std::vector< std::string >& requiredAttributesVector, const std::vector< std::string >& optionalAttributesVector )
	{
		std::string errorMessage;
		std::vector< bool > requiredAttributePresent( requiredAttributesVector.size(), false );

		std::string requiredAttributesString;
		std::string optionalAttributesString;

		csvStringFromVector( requiredAttributesString, requiredAttributesVector );
		csvStringFromVector( optionalAttributesString, optionalAttributesVector );

		//iterate through all of the current node's attributes
		stringVectorCompareResult result;
		for( auto attributeLocation = currentNode.m_currentNode.attributes_begin(); attributeLocation != currentNode.m_currentNode.attributes_end(); ++attributeLocation )
		{
			//search on the required list of attributes
			result = compareStringToVector( requiredAttributesVector, attributeLocation->name(), &stringCompareFunction );

			if( result.m_compareResult == MATCH )
			{
				requiredAttributePresent[ result.m_vectorMatchIndex ] = true;
				continue;
			}
			else if( result.m_compareResult == MISSMATCH )
			{
				//we have a case missmatch
				errorMessage = "Unexpected Attribute: \n\n";

				errorMessage += std::string("Found unexpected attribute ") + "\'" + attributeLocation->name() + "\'" + "\n";
				errorMessage = errorMessage + "Attribute names are case sensitive, did you mean " + "\'" + requiredAttributesVector[ result.m_vectorMatchIndex ] + "\'" +"?\n";
				errorMessage = errorMessage + "Attributes allowed in elements of type " + currentNode.m_currentNode.name() + " are the following:\n\n";

				errorMessage = errorMessage + "Required Attributes:\n";
				errorMessage = errorMessage + requiredAttributesString + "\n\n";

				errorMessage = errorMessage + "Optional Attributes:\n";
				errorMessage = errorMessage + optionalAttributesString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentNode.m_currentNode.name() + "\n";
				errorMessage = errorMessage + "Attribute: " + attributeLocation->name();

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Attribute Not Found" + "\n";
				ErrorReporter::throwError( "Attribute Not Found", errorMessage, vsOutput );

			}

			//search on the optional vector
			result = compareStringToVector( optionalAttributesVector, attributeLocation->name(), &stringCompareFunction );

			if( result.m_compareResult == MATCH )
			{
				continue;
			}
			else if( result.m_compareResult == MISSMATCH )
			{
				//we have a case missmatch
				errorMessage = "Unexpected Attribute: \n\n";

				errorMessage = errorMessage + "Found unexpected attribute " + "\'" + attributeLocation->name() + "\'" + "\n";
				errorMessage = errorMessage + "Attribute names are case sensitive, did you mean " + "\'" + optionalAttributesVector[ result.m_vectorMatchIndex ] + "\'" +"?\n";
				errorMessage = errorMessage + "Attributes allowed in elements of type " + currentNode.m_currentNode.name() + " are the following:\n\n";

				errorMessage = errorMessage + "Required Attributes:\n";
				errorMessage = errorMessage + requiredAttributesString + "\n\n";

				errorMessage = errorMessage + "Optional Attributes:\n";
				errorMessage = errorMessage + optionalAttributesString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentNode.m_currentNode.name() + "\n";
				errorMessage = errorMessage + "Attribute: " + attributeLocation->name();

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Attribute Not Found" + "\n";
				ErrorReporter::throwError( "Attribute Not Found", errorMessage, vsOutput );

			}
			else
			{
				errorMessage = "Unexpected Attribute: \n\n";

				errorMessage = errorMessage + "Found unexpected attribute " + "\'" + attributeLocation->name() + "\'" + "\n";
				errorMessage = errorMessage + "Attributes allowed in elements of type " + currentNode.m_currentNode.name() + " are the following:\n\n";

				errorMessage = errorMessage + "Required Attributes:\n";
				errorMessage = errorMessage + requiredAttributesString + "\n\n";

				errorMessage = errorMessage + "Optional Attributes:\n";
				errorMessage = errorMessage + optionalAttributesString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentNode.m_currentNode.name() + "\n";
				errorMessage = errorMessage + "Attribute: " + attributeLocation->name();

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Attribute Not Found" + "\n";
				ErrorReporter::throwError( "Attribute Not Found", errorMessage, vsOutput );
			}
		}

		//iterate to see that all the required attributes are present
		for( auto requiredAttributeIter = requiredAttributePresent.begin(); requiredAttributeIter != requiredAttributePresent.end(); ++requiredAttributeIter )
		{
			if( !( *requiredAttributeIter ) )
			{
				errorMessage = "Missing Required Attribute: \n\n";

				errorMessage = errorMessage + "Element missing required attribute " + "\'" + requiredAttributesVector[ requiredAttributeIter - requiredAttributePresent.begin() ] + "\'" + "\n";
				errorMessage = errorMessage + "Elements of type " + currentNode.m_currentNode.name() + " are required the following attributes:\n\n";
				errorMessage = errorMessage + requiredAttributesString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentNode.m_currentNode.name() + "\n";
				errorMessage = errorMessage + "Missing Attribute: " + requiredAttributesVector[ requiredAttributeIter - requiredAttributePresent.begin() ];

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Missing Attribute" + "\n";
				ErrorReporter::throwError( "Missing Attribute", errorMessage, vsOutput );
			}
		}

		return true;
	}


	void XMLNode::removeWhiteSpace( std::string& line, bool front )
	{
		size_t index = std::string::npos;

		if( front )
		{
			index = line.find_first_not_of( " \t" );
			if( index == std::string::npos )
			{
				line.clear();
			}
			else
			{
				line.erase( 0, index );
			}
		}
		else
		{
			index = line.find_last_not_of( " \t" );
			if( index == std::string::npos )
			{
				line.clear();
			}
			else
			{
				line = line.substr( 0, index + 1 );
			}
		}
	}


	void XMLNode::removeFrontAndBackWhiteSpace( std::string& textBlock )
	{
		removeWhiteSpace( textBlock, true );
		removeWhiteSpace( textBlock, false );
	}

	void XMLNode::separateKeyList( const std::string& listOfKeys, std::vector< std::string >& keyContainer )
	{
		size_t commaLocation = 0;
		size_t startIndex = 0;
		size_t currentIndex = 0;
		std::string substring;

		commaLocation = listOfKeys.find( ',', currentIndex );
		while ( commaLocation != std::string::npos )
		{
			substring = listOfKeys.substr( startIndex, commaLocation-startIndex );

			//remove whitespace
			removeFrontAndBackWhiteSpace( substring );

			//if the string is not empty, then we add it to the container of keys
			if( !substring.empty() )
			{
				keyContainer.push_back( substring );
			}

			startIndex = commaLocation+1;
			commaLocation = listOfKeys.find( ',', startIndex );
		}

		if( startIndex < listOfKeys.size() )
		{
			substring = listOfKeys.substr( startIndex );
			
			//remove whitespace
			removeFrontAndBackWhiteSpace( substring );

			//if the string is not empty, then we add it to the container of keys
			if( !substring.empty() )
			{
				keyContainer.push_back( substring );
			}
		}
	}

	bool XMLNode::ValidateXMLChildElements( const XMLNode& currentNode, const std::string& requiredChildElements, const std::string& optionalChildElements )
	{
		std::vector< std::string > requiredChildrenVector;
		std::vector< std::string > optionalChildrenVector;

		separateKeyList( requiredChildElements, requiredChildrenVector );
		separateKeyList( optionalChildElements, optionalChildrenVector );

		return ValidateXMLChildElements( currentNode, requiredChildrenVector, optionalChildrenVector );
	}

	bool XMLNode::ValidateXMLChildElements( const XMLNode& currentNode, const std::vector< std::string >& requiredChildrenVector, const std::vector< std::string >& optionalChildrenVector )
	{
		std::string errorMessage;
		std::vector< bool > requiredChildPresent( requiredChildrenVector.size(), false );

		std::string requiredChildrenString;
		std::string optionalChildrenString;

		csvStringFromVector( requiredChildrenString, requiredChildrenVector );
		csvStringFromVector( optionalChildrenString, optionalChildrenVector );

		//iterate through all of the current node's children
		bool matchFound = false;
		stringVectorCompareResult result;

		for( auto currentChildToCheck = currentNode.m_currentNode.first_child(); currentChildToCheck != NULL; currentChildToCheck = currentChildToCheck.next_sibling() )
		{
			matchFound = false;
			//search on the required list of children

			result = compareStringToVector( requiredChildrenVector, currentChildToCheck.name(), &stringCompareFunction );

			if( result.m_compareResult == MATCH )
			{
				requiredChildPresent[ result.m_vectorMatchIndex ] = true;
				continue;
			}
			else if( result.m_compareResult == MISSMATCH )
			{
				errorMessage = "Unexpected Child Element: \n\n";

				errorMessage = errorMessage + "Found unexpected child element " + "\'" + currentChildToCheck.name() + "\'" + "\n";
				errorMessage = errorMessage + "The type of child elements are case sensitive, did you mean " + "\'" + requiredChildrenVector[ result.m_vectorMatchIndex ] + "\'" +"?\n";
				errorMessage = errorMessage + "Child elements allowed are the following:\n\n";

				errorMessage = errorMessage + "Required Children:\n";
				errorMessage = errorMessage + requiredChildrenString + "\n\n";

				errorMessage = errorMessage + "Optional Children:\n";
				errorMessage = errorMessage + optionalChildrenString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentChildToCheck.name() + "\n";

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Unexpected Child Element" + "\n";

				ErrorReporter::throwError( "Unexpected Child Element", errorMessage, vsOutput );
			}

			result = compareStringToVector( optionalChildrenVector, currentChildToCheck.name(), &stringCompareFunction );
			if( result.m_compareResult == MATCH )
			{
				continue;
			}
			else if( result.m_compareResult == MISSMATCH )
			{
				errorMessage = "Unexpected Child Element: \n\n";

				errorMessage = errorMessage + "Found unexpected child element " + "\'" + currentChildToCheck.name() + "\'" + "\n";
				errorMessage = errorMessage + "The type of child elements are case sensitive, did you mean " + "\'" + optionalChildrenVector[ result.m_vectorMatchIndex ] + "\'" +"?\n";
				errorMessage = errorMessage + "Child elements allowed are the following:\n\n";

				errorMessage = errorMessage + "Required Children:\n";
				errorMessage = errorMessage + requiredChildrenString + "\n\n";

				errorMessage = errorMessage + "Optional Children:\n";
				errorMessage = errorMessage + optionalChildrenString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentChildToCheck.name() + "\n";

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Unexpected Child Element" + "\n";

				ErrorReporter::throwError( "Unexpected Child Element", errorMessage, vsOutput );
			}
			else
			{
				errorMessage = "Unexpected Child Element: \n\n";

				errorMessage = errorMessage + "Found unexpected child element " + "\'" + currentChildToCheck.name() + "\'" + "\n";
				errorMessage = errorMessage + "Child elements allowed are the following:\n\n";

				errorMessage = errorMessage + "Required Children:\n";
				errorMessage = errorMessage + requiredChildrenString + "\n\n";

				errorMessage = errorMessage + "Optional Children:\n";
				errorMessage = errorMessage + optionalChildrenString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Element Type: " + currentChildToCheck.name() + "\n";
				
				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Unexpected Child Element" + "\n";

				ErrorReporter::throwError( "Unexpected Child Element", errorMessage, vsOutput );
			}
		}

		//iterate to see that all the required attributes are present
		for( auto requiredChildIter = requiredChildPresent.begin(); requiredChildIter != requiredChildPresent.end(); ++requiredChildIter )
		{
			if( !( *requiredChildIter ) )
			{
				errorMessage = "Missing Required Child Element: \n\n";

				errorMessage = errorMessage + "XML Document missing required child element " + "\'" + requiredChildrenVector[ requiredChildIter - requiredChildPresent.begin() ] + "\'" + "\n";
				errorMessage = errorMessage + "The following elements are required in the document:\n\n";
				errorMessage = errorMessage + requiredChildrenString + "\n\n";

				errorMessage = errorMessage + "Error Details:\n";
				errorMessage = errorMessage + "File Name: " + currentNode.m_fileName + "\n";
				errorMessage = errorMessage + "Missing Element: " + requiredChildrenVector[ requiredChildIter - requiredChildPresent.begin() ] + "\n";

				std::string vsOutput = currentNode.m_filePath + "\n";
				vsOutput =	vsOutput + "\t> " + "Missing Child Element" + "\n";

				ErrorReporter::throwError( "Missing Child Element", errorMessage, vsOutput );
			}
		}

		return true;
	}

	bool XMLNode::attributeIsPresent( std::string attributeName ) const
	{
		return m_currentNode.attribute( attributeName.c_str() ) != NULL;
	}

	bool XMLNode::hasAttributes() const
	{
		return m_currentNode.first_attribute() != NULL;
	}

	void XMLNode::forEachChild( std::function< void ( const XMLNode& ) > func ) const
	{
		XMLNode temporaryNode;

		if( !m_currentNode.empty() )
		{
			pugi::xml_node currentChild = m_currentNode.first_child();

			if( currentChild != NULL )
			{
				for( bool applyFunction = true; applyFunction; applyFunction = !currentChild.empty() )
				{
					temporaryNode.m_currentNode = currentChild;
					func( temporaryNode );
					currentChild = currentChild.next_sibling();
				}
			}
		}
	}

	bool XMLNode::hasChildren() const
	{
		return m_currentNode.first_child() != NULL;
	}

	std::string XMLNode::getFileName() const
	{
		return m_fileName;
	}

	std::string XMLNode::getElementName() const
	{
		return m_currentNode.name();
	}

	std::string XMLNode::getFilePath() const
	{
		return m_filePath;
	}

	stringVectorCompareResult XMLNode::compareStringToVector( const std::vector< std::string >& vectorToIterate, const std::string& stringToCompare, stringVectorCompareResult (*compareFunction)( const std::string&, const std::string& ) )
	{
		stringVectorCompareResult currentResult;
		for( auto currentValueIter = vectorToIterate.begin(); currentValueIter != vectorToIterate.end(); ++currentValueIter )
		{
			currentResult = compareFunction( stringToCompare, *currentValueIter );
			if( currentResult.m_compareResult != NOMATCH )
			{
				currentResult.m_vectorMatchIndex = currentValueIter - vectorToIterate.begin();
				break;
			}
		}

		return currentResult;
	}

	stringVectorCompareResult XMLNode::stringCompareFunction( const std::string& value1, const std::string& value2 )
	{
		stringVectorCompareResult result;

		if( value1 == value2 )
		{
			result.m_compareResult = MATCH;
		}
		else
		{
			//check if the case of characters cause the missmatch
			std::string value1Lowercase = value1;
			std::string value2Lowercase = value2;

			std::transform( value1Lowercase.begin(), value1Lowercase.end(), value1Lowercase.begin(), ::tolower );
			std::transform( value2Lowercase.begin(), value2Lowercase.end(), value2Lowercase.begin(), ::tolower );

			if( value1Lowercase == value2Lowercase )
			{
				result.m_compareResult = MISSMATCH;
			}
		}

		return result;
	}

	Rgba XMLNode::GetRgbaXMLAttribute( const XMLNode& currentNode, std::string attributeName, Rgba defaultValue )
	{
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		if( attributeLocation != currentNode.m_currentNode.attributes_end())
		{
			std::string rgbaCSValues = attributeLocation->as_string("");
			std::vector< std::string > rgbaValues;
			separateKeyList(rgbaCSValues, rgbaValues);

			if( rgbaValues.size() == 3 )
			{
				return Rgba( (float)atof( rgbaValues[0].c_str() ), (float)atof( rgbaValues[1].c_str() ), (float)atof( rgbaValues[2].c_str() ) );
			}
			else if( rgbaValues.size() == 4 )
			{
				return Rgba( (float)atof( rgbaValues[0].c_str() ), (float)atof( rgbaValues[1].c_str() ), (float)atof( rgbaValues[2].c_str() ), (float)atof( rgbaValues[3].c_str() ) );
			}
			else
			{
				return defaultValue;
			}
		}
		else
		{
			return defaultValue;
		}
	}

	IntVector2 XMLNode::GetIntVector2XMLAttribute( const XMLNode& currentNode, std::string attributeName, IntVector2 defaultValue )
	{
		//find the location of the attribute we are to retrieve
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		//if we have a valid location for the attribute, then we retrieve it
		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return as_IntVec2( attributeLocation->as_string(), defaultValue );
		}
		else
		{
			return defaultValue;
		}
	}

	bool XMLNode::GetBoolXMLAttribute( const XMLNode& currentNode, std::string attributeName, bool defaultValue )
	{
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			return attributeLocation->as_bool( defaultValue );
		}
		else
		{
			return defaultValue;
		}
	}

	char XMLNode::GetCharXMLAttribute( const XMLNode& currentNode, std::string attributeName, char defaultValue )
	{
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		std::string charHolder;

		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			charHolder = attributeLocation->as_string("");
			removeFrontAndBackWhiteSpace(charHolder);
			if( charHolder.empty() )
			{
				return defaultValue;
			}
			else
			{
				return charHolder.front();
			}
		}
		else
		{
			return defaultValue;
		}
	}

	FloatRange XMLNode::GetFloatRangeAttribute( const XMLNode& currentNode, const std::string& attributeName, const std::string& defaultValueRange )
	{
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		FloatRange toReturn( defaultValueRange );

		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			toReturn = FloatRange( attributeLocation->as_string( defaultValueRange.c_str() ) );
		}

		return toReturn;
	}

	IntRange XMLNode::GetIntRangeAttribute( const XMLNode& currentNode, const std::string& attributeName, const std::string& defaultValueRange )
	{
		pugi::xml_attribute_iterator attributeLocation = FindAttributeLocation( currentNode.m_currentNode, attributeName );

		IntRange toReturn( defaultValueRange );

		if( attributeLocation != currentNode.m_currentNode.attributes_end() )
		{
			toReturn = IntRange( attributeLocation->as_string( defaultValueRange.c_str() ) );
		}

		return toReturn;
	}

	void XMLNode::forChild( const std::string& childName, std::function< void ( const XMLNode& ) > func ) const
	{
		XMLNode temporaryNode;

		if( !m_currentNode.empty() )
		{
			pugi::xml_node currentChild = m_currentNode.child( childName.c_str() );

			if( currentChild != NULL )
			{
				temporaryNode.m_currentNode = currentChild;
				func( temporaryNode );
			}
		}
	}

}