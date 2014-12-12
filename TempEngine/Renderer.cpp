#include <algorithm>
#include <glew.h>

#include "Renderer.h"
#include "pugixml.hpp"
#include "BitmapFont.h"
#include "TextureManager.h"
#include "Texture.h"
#include "MatrixStack.h"
#include "MathUtilities.hpp"
#include "Vector4.h"
#include "XMLNode.h"

//#include "MemoryUtilities.h"

gh::Renderer theRenderer;

namespace gh
{
	std::shared_ptr< BitmapFont > Renderer::loadFont( const char* fontDefinitionXML, const std::string& fontName, const std::vector< std::string >& glyphSheetsLocations )
	{
		bitmapFontsIterator iter = m_bitmapFonts.find( fontName );

		//check if we do not have the font
		if( iter == m_bitmapFonts.end() )
		{
			//load the glyphSheets
			std::vector< std::shared_ptr< Texture > > glyphSheets;

			for( auto currentLocation = glyphSheetsLocations.begin(); currentLocation != glyphSheetsLocations.end(); ++currentLocation )
			{
				glyphSheets.push_back( TextureManager::instance().getTexture( currentLocation->c_str() ) );
			}

			//read in the xml file and parse it in order to get the font definition
			XMLNode doc2( fontDefinitionXML );
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file( doc2.m_filePath.c_str() );

			//reading the pixelHeight of each cell
			//doc2.goToChild( "FontDefinition" );
			doc2.goToChild( "FontInfo" );
			float cellPixelHeight = XMLNode::GetFloatXMLAttribute( doc2, "cellPixelHeight", 10.f );
			//float cellPixelHeight = doc.child("FontDefinition").child("FontInfo").attribute("cellPixelHeight").as_float();

			//calculating the max UCS index so that we can create our array of character data
			doc2.goToRoot();
			doc2.goToChild( "Glyph" );
			int maxUCS = XMLNode::GetIntXMLAttribute( doc2, "ucsIndex", 0 );

			//int maxUCS = doc.child( "FontDefinition" ).child("Glyph").attribute( "ucsIndex" ).as_int();

			doc2.goToRoot();
			doc2.forEachChild( [&]( const XMLNode& currentNode )
			{
				int x = 1;
				//std::string myName = currentNode.name();
				if( currentNode.getElementName() == "Glyph" )
				{
					int ucsVal = XMLNode::GetIntXMLAttribute( currentNode, "ucsIndex", 0 );
					maxUCS = maxUCS > ucsVal ? maxUCS : ucsVal;
				}
			});

			/*for (pugi::xml_node currentNode = doc.child("FontDefinition").child("Glyph"); currentNode; currentNode = currentNode.next_sibling())
			{
				std::string myName = currentNode.name();
				if( myName == "Glyph" )
				{
					int ucsVal = currentNode.attribute( "ucsIndex" ).as_int();
					maxUCS = maxUCS > ucsVal ? maxUCS : ucsVal;
				}
			}*/

			//read in the character data of all the characters defined in the xml file
			std::vector< CharData >fontCharacters( maxUCS + 1 );

			doc2.goToRoot();
			doc2.forEachChild( [&]( const XMLNode& currentNode )
			{
				CharData toAdd;
				//std::string myName = currentNode.name();
				if( currentNode.getElementName() == "Glyph" )
				{
					toAdd.C = XMLNode::GetFloatXMLAttribute( currentNode, "ttfC", 0.f );
					toAdd.B = XMLNode::GetFloatXMLAttribute( currentNode, "ttfB", 0.f );
					toAdd.A = XMLNode::GetFloatXMLAttribute( currentNode, "ttfA", 0.f );

					toAdd.texCoordMaxs = XMLNode::GetVector2XMLAttribute( currentNode, "texCoordMaxs", Vector2() );
					toAdd.texCoordMins = XMLNode::GetVector2XMLAttribute( currentNode, "texCoordMins", Vector2() );

					toAdd.glyphSheetNum = XMLNode::GetIntXMLAttribute( currentNode, "sheet", 0 );
					fontCharacters[ XMLNode::GetIntXMLAttribute( currentNode, "ucsIndex", 0 ) ] = toAdd;
				}
			});

			/*for (pugi::xml_node currentNode = doc.child("FontDefinition").child("Glyph"); currentNode; currentNode = currentNode.next_sibling())
			{
				CharData toAdd;
				std::string myName = currentNode.name();
				if( myName == "Glyph" ) 
				{
					for (pugi::xml_attribute attr = currentNode.first_attribute(); attr; attr = attr.next_attribute() )
					{
						toAdd.C = currentNode.attribute("ttfC").as_float();
						toAdd.B = currentNode.attribute("ttfB").as_float();
						toAdd.A = currentNode.attribute("ttfA").as_float();

						toAdd.texCoordMaxs = as_Vec2( currentNode.attribute("texCoordMaxs").as_string() );
						toAdd.texCoordMins = as_Vec2( currentNode.attribute("texCoordMins").as_string() );

						toAdd.glyphSheetNum = currentNode.attribute("sheet").as_int();

						fontCharacters[ currentNode.attribute("ucsIndex").as_int() ] = toAdd;
					}
				}
			}*/

			std::shared_ptr< BitmapFont > newFont = std::make_shared< BitmapFont >( glyphSheets, fontCharacters, cellPixelHeight );
			m_bitmapFonts.insert( std::pair< std::string, std::shared_ptr< BitmapFont > >( fontName, newFont ) );
			return newFont;
		}
		else
		{
			return iter->second;
		}
	}

	Vector2 Renderer::as_Vec2( const char* vector2Values )
	{
		bool prevEmpty = true;
		size_t startIndex = 0;

		char* subbuff = NULL;

		size_t vectorIndex = 0;

		Vector2 toReturn;

		bool keepReading = true;
		for( size_t index = 0; keepReading; ++index )
		{
			switch( *( vector2Values + index ) )
			{
			case ' ':
				{
					if( !prevEmpty )
					{
						subbuff = new char[ index - startIndex ];
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

						prevEmpty = true;
					}
				}
				break;

			case 0:
				{
					if( !prevEmpty )
					{
						subbuff = new char[ index - startIndex ];
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

						prevEmpty = true;

						keepReading = false;
					}
				}
				break;

			case',':
				{
					if( !prevEmpty )
					{
						subbuff = new char[ index - startIndex ];
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

						prevEmpty = true;
					}
				}
				break;

			default:
				{
					//not an emptySpace
					if( prevEmpty )
					{
						startIndex = index;
						prevEmpty = false;
					}
				}
			}

		}

		return toReturn;
	}

	void Renderer::drawText( const char* outputString, const std::string& fontName, float pixelHeight, const AABB2& renderQuad, unsigned char alignment /*= BOX_ALIGN*/, unsigned int textTint /* = 0x0 */ )
	{
		//look for the font
		std::shared_ptr< BitmapFont > renderFont = getFont( fontName );

		if( renderFont == NULL || outputString == nullptr || *outputString == 0 )
		{
			//return if the font is not found or there is nothing to render
			return;
		}

		Vector2 textLocation = renderQuad.getMins();

		//calculating the alignment of the text
		alignText( textLocation, renderQuad, alignment, outputString, pixelHeight, renderFont );

		//now that we have our starting location, we start rendering
		Vector2 prevBottomRight = textLocation;
		for( size_t index = 0; *( outputString + index ); ++index )
		{
			//already have the bottomLeft need the topRight
			//add A
			Vector2 bottomLeft( prevBottomRight.x + renderFont->getCharData( *( outputString + index ) ).A * pixelHeight, textLocation.y );
			Vector2 topRight( prevBottomRight.x + renderFont->getCharData( *( outputString + index ) ).calculatePixelWidth( pixelHeight ), prevBottomRight.y + pixelHeight );

			Vector2 minTexCoords = renderFont->getCharData( *( outputString + index ) ).texCoordMins;
			Vector2 maxTexCoords = renderFont->getCharData( *( outputString + index ) ).texCoordMaxs;
			Vector2 temp( minTexCoords.y, maxTexCoords.y );

			minTexCoords.y = temp.y;
			maxTexCoords.y = temp.x;

			pushCharacterTextData( bottomLeft, topRight, minTexCoords, maxTexCoords, textTint, m_textDataArray );

			prevBottomRight.x = topRight.x;
			prevBottomRight.y = bottomLeft.y;
		}

		//rendering the text we just created
		renderDataArrayV2PTC( m_textDataArray, fontName );

		m_textDataArray.clear();
	}


	void Renderer::pushCharacterTextData( const Vector2& bottomLeft, const Vector2&topRight, const Vector2& minTexCoords, const Vector2& maxTexCoords, unsigned int textTint, std::vector< Vertex2PTC >& textDataArray )
	{
		Vertex2PTC toPush;

		toPush.x = bottomLeft.x;
		toPush.y = bottomLeft.y;
		toPush.u = minTexCoords.x;
		toPush.v = minTexCoords.y;
		toPush.RGBA = textTint;
		textDataArray.push_back( toPush );

		toPush.x = topRight.x;
		toPush.y = bottomLeft.y;
		toPush.u = maxTexCoords.x;
		toPush.v = minTexCoords.y;
		toPush.RGBA = textTint;
		textDataArray.push_back( toPush );

		toPush.x = topRight.x;
		toPush.y = topRight.y;
		toPush.u = maxTexCoords.x;
		toPush.v = maxTexCoords.y;
		toPush.RGBA = textTint;
		textDataArray.push_back( toPush );
		
		
		textDataArray.push_back( toPush );

		toPush.x = bottomLeft.x;
		toPush.y = topRight.y;
		toPush.u = minTexCoords.x;
		toPush.v = maxTexCoords.y;
		toPush.RGBA = textTint;
		textDataArray.push_back( toPush );

		toPush.x = bottomLeft.x;
		toPush.y = bottomLeft.y;
		toPush.u = minTexCoords.x;
		toPush.v = minTexCoords.y;
		toPush.RGBA = textTint;
		textDataArray.push_back( toPush );
	}

	void Renderer::alignText( Vector2& textLocation, const AABB2& renderQuad, unsigned int alignment, const char* outputString, float pixelHeight, const std::shared_ptr<BitmapFont>& renderFont )
	{
		//WORLD_ALIGN means that the text will be aligned to the quad given
		if( BOX_ALIGN & alignment )
		{
			//Determining the start location
			if( TEXT_ALIGN_RIGHT & alignment )
			{
				//align right
				textLocation.x = renderQuad.getMaxs().x - renderFont->calcTextPixelWidth( outputString, pixelHeight ); 
			}
			if( TEXT_ALIGN_CENTERX & alignment )
			{
				//align to centerX
				textLocation.x = ( ( renderQuad.getMaxs().x + renderQuad.getMins().x ) * .5f ) - ( renderFont->calcTextPixelWidth( outputString, pixelHeight ) * .5f );
			}
			if( TEXT_ALIGN_TOP & alignment )
			{
				//align to top
				textLocation.y = renderQuad.getMaxs().y - pixelHeight;
			}
			if( TEXT_ALIGN_CENTERY & alignment )
			{
				//align to the center in Y
				textLocation.y = ( ( renderQuad.getMaxs().y + renderQuad.getMins().y ) * .5f ) - ( pixelHeight * .5f ); 
			}
		}
		//if it is not world aligned, it uses the quads minCoords to align itself
		else
		{
			//Determining the start location
			if( TEXT_ALIGN_RIGHT & alignment )
			{
				//align right 
				textLocation.x = renderQuad.getMins().x - renderFont->calcTextPixelWidth( outputString, pixelHeight ); 
			}
			if( TEXT_ALIGN_CENTERX & alignment )
			{
				//align to centerX
				textLocation.x = ( renderQuad.getMins().x * .5f ) - ( renderFont->calcTextPixelWidth( outputString, pixelHeight ) * .5f );
			}
			if( TEXT_ALIGN_TOP & alignment )
			{
				//align to top
				textLocation.y = renderQuad.getMins().y - pixelHeight;
			}
			if( TEXT_ALIGN_CENTERY & alignment )
			{
				//align to the center in Y
				textLocation.y = ( renderQuad.getMins().y * .5f ) - ( pixelHeight * .5f ); 
			}
		}
	}

	std::vector< Vertex2PTC > Renderer::generateTextDataArray( std::vector< CommandConsoleText > textLines, std::string fontName, float fontHeight, const AABB2& renderQuad, unsigned char alignment, int renderDirection )
	{
		std::vector< Vertex2PTC > textDataArray;

		//look for the font
		std::shared_ptr< BitmapFont > renderFont = getFont( fontName );

		if( renderFont == NULL || textLines.empty() )
		{
			//return the empty vector f the font is not found or the textLines data is empty
			return textDataArray;
		}

		//determining alignment and how many lines we can render
		Vector2 textLocation = renderQuad.getMins();

		if( TEXT_ALIGN_TOP & alignment )
		{
			//align to top
			textLocation.y = renderQuad.getMaxs().y - fontHeight;
		}

		int maxNumberOfLines = 0;
		if( renderDirection == -1 )
		{
			maxNumberOfLines = (int)( ( textLocation.y + fontHeight - renderQuad.getMins().y ) / fontHeight );
		}
		else
		{
			maxNumberOfLines = (int)( ( renderQuad.getMaxs().y - textLocation.y ) / fontHeight );
		}
		//if the max number of lines we can render is less than or equal to 0, then we don't render
		if( maxNumberOfLines <= 0 )
		{
			return textDataArray;
		}

		//calculating how many lines we can render
		int linesToRender = (int)textLines.size() >= maxNumberOfLines ? maxNumberOfLines : textLines.size();
		
		//offsetting the starting position
		Vector2 prevBottomRight = textLocation;
		prevBottomRight.y += ( ( linesToRender - 1 ) * fontHeight * renderDirection );

		//calculating the index of the oldest line we are to render
		int index = ( textLines.size() - linesToRender );

		//iterating through all the lines of text
		for( auto iter = textLines.begin() + index; iter != textLines.end(); ++iter )
		{
			//extracting the output text and tint
			std::string outputString = iter->m_textLine;
			unsigned int textTint = iter->m_textTint;

			//iterating through all the characters in the string
			for( auto innerIter = outputString.begin(); innerIter != outputString.end(); ++innerIter )
			{
				Vector2 bottomLeft( prevBottomRight.x + renderFont->getCharData( *( innerIter ) ).A * fontHeight, prevBottomRight.y );
				Vector2 topRight( prevBottomRight.x + renderFont->getCharData( *( innerIter ) ).calculatePixelWidth( fontHeight ), prevBottomRight.y + fontHeight );

				Vector2 minTexCoords = renderFont->getCharData( *( innerIter ) ).texCoordMins;
				Vector2 maxTexCoords = renderFont->getCharData( *( innerIter ) ).texCoordMaxs;
				Vector2 temp( minTexCoords.y, maxTexCoords.y );

				minTexCoords.y = temp.y;
				maxTexCoords.y = temp.x;

				pushCharacterTextData( bottomLeft, topRight, minTexCoords, maxTexCoords, textTint, textDataArray );

				prevBottomRight.x = topRight.x;
				prevBottomRight.y = bottomLeft.y;
			}

			//moving the starting location of the next line
			prevBottomRight.x = renderQuad.getMins().x;
			prevBottomRight.y -= fontHeight * renderDirection;
		}

		return textDataArray;
	}

	void Renderer::renderDataArrayV2PTC( const std::vector< Vertex2PTC >& textDataArray, const std::string& fontName )
	{
		//find the font
		std::shared_ptr< BitmapFont > renderFont = getFont( fontName );

		if( renderFont == NULL )
		{
			//don't render if we did not find the font
			return;
		}
		//else, apply the texture and render the text data
		else
		{
			renderFont->applyTexture();
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2PTC ), &(textDataArray[0]) );
		glEnableClientState( GL_VERTEX_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2PTC ), &(textDataArray[0].u) );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex2PTC ), &(textDataArray[0].RGBA ) );
		glEnableClientState( GL_COLOR_ARRAY );
		glDrawArrays( GL_TRIANGLES, 0, textDataArray.size() );

		glDisable( GL_TEXTURE_COORD_ARRAY );
		glDisable( GL_VERTEX_ARRAY );
		glDisable( GL_COLOR_ARRAY );
		glDisable( GL_TEXTURE_2D );
	}

	void Renderer::clearText()
	{
		m_textDataArray.clear();
	}

	std::shared_ptr< BitmapFont > Renderer::getFont( const std::string& fontName )
	{
		bitmapFontsIterator iter = m_bitmapFonts.find( fontName );

		//check if we found the font
		if( iter == m_bitmapFonts.end() )
		{
			return NULL;
		}
		
		return iter->second;
	}

	void Renderer::renderQuad( const AABB2& windowSize, const Vector4& tint, std::shared_ptr< Texture >backgroundTexture )
	{
		glColor4f( tint.x, tint.y, tint.z, tint.w );
		
		if( backgroundTexture )
		{
			backgroundTexture->apply();
		}

		glBegin( GL_QUADS );

		if( backgroundTexture )
		{
			glVertex2f( windowSize.getMins().x, windowSize.getMins().y );
			glTexCoord2f( 0.f, 0.f );
			glVertex2f( windowSize.getMaxs().x, windowSize.getMins().y );
			glTexCoord2f( 1.f, 0.f );
			glVertex2f( windowSize.getMaxs().x, windowSize.getMaxs().y );
			glTexCoord2f( 1.f, 1.f );
			glVertex2f( windowSize.getMins().x, windowSize.getMaxs().y );
			glTexCoord2f( 0.f, 1.f );
		}
		else
		{
			glVertex2f( windowSize.getMins().x, windowSize.getMins().y );
			glVertex2f( windowSize.getMaxs().x, windowSize.getMins().y );
			glVertex2f( windowSize.getMaxs().x, windowSize.getMaxs().y );
			glVertex2f( windowSize.getMins().x, windowSize.getMaxs().y );
		}
		
		glEnd();
		glColor4f( 1.f, 1.f, 1.f, 1.f );
	}

	void Renderer::bindVBO( unsigned int idVBO )
	{
		glBindBuffer( GL_ARRAY_BUFFER, idVBO );
	}
	void Renderer::drawTriangles( unsigned int idIBO, unsigned int numIndices )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idIBO );
		glDrawElements( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}

	unsigned int Renderer::generateIBO( size_t sizeOfDataBytes, const void* data )
	{
		unsigned int IBOid = 0;

		glGenBuffers( 1, &( IBOid ) );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBOid );

		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeOfDataBytes, data, GL_STATIC_DRAW );

		return IBOid;
	}

	void Renderer::overwriteVBO( unsigned int& idVBO, size_t sizeOfDataBytes, const void* data )
	{
		//checking if there is already a VBO created
		if( idVBO != 0 )
		{
			//delete the previous buffer
			glDeleteBuffers( 1, &idVBO );
		}

		//generate the VBO
		glGenBuffers( 1, &( idVBO ) );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO );

		glBufferData( GL_ARRAY_BUFFER, sizeOfDataBytes, data, GL_STATIC_DRAW );
	}

	void Renderer::useRegularFBO( const Vector4& clearColor, unsigned long clearBits )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
		glClear( clearBits );
	}

	void Renderer::updateWindowSize( const Vector2& windowSize )
	{
		m_windowSize = windowSize;
	}

	Vector2 Renderer::getWindowSize()
	{
		return m_windowSize;
	}

	void Renderer::disableAllTextures()
	{
		glActiveTexture( GL_TEXTURE5 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE4 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE3 );
		glBindTexture( GL_TEXTURE_2D, 0);
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0);
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0);
		glDisable( GL_TEXTURE_2D );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0);
		glDisable( GL_TEXTURE_2D );

		glDisable( GL_DEPTH_TEST );
	}

}