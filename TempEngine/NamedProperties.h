#pragma once
#include <map>
#ifndef NAMED_PROPERTIES_H
#define NAMED_PROPERTIES_H

#include "NamedProperty.h"

namespace gh
{
	enum PropertyGetResult{ PROPERTY_GET_SUCCESS = 0, PROPERTY_GET_FAILED_WRONG_TYPE, PROPERTY_GET_FAILED_NO_SUCH_PROPERTY };
	
	class NamedProperties
	{
	public:
		template < typename T_propertyType >
		void setProperty( const std::string& nameOfProperty, const T_propertyType& data )
		{
			std::map< std::string, NamedPropertyBase* >::iterator found = m_properties.find( nameOfProperty );
			if( found != m_properties.end() )
			{
				NamedPropertyBase* item = found->second;
				delete item;
			}
			m_properties[ nameOfProperty ] = new NamedProperty< T_propertyType >( data );
		}

		void setProperty( const std::string& nameOfProperty, const char* data )
		{
			setProperty(nameOfProperty, std::string( data ) );
		}

		void setProperty( const std::string& nameOfProperty, char* data )
		{
			setProperty(nameOfProperty, std::string( data ) );
		}

		template< typename T_propertyType >
		PropertyGetResult getProperty( const std::string& nameOfProperty, T_propertyType& outData ) const
		{
			std::map< std::string, NamedPropertyBase* >::const_iterator found = m_properties.find( nameOfProperty );
			if( found != m_properties.end() )
			{
				NamedPropertyBase* npBase = found->second;
				NamedProperty< T_propertyType >* typedProperty = dynamic_cast< NamedProperty< T_propertyType >* >( npBase );

				if( typedProperty )
				{
					outData = typedProperty->m_data;
					return PROPERTY_GET_SUCCESS;
				}
				else
				{
					return PROPERTY_GET_FAILED_WRONG_TYPE;
				}
			}
			else
			{
				return PROPERTY_GET_FAILED_NO_SUCH_PROPERTY;
			}
		}

	private:
		std::map< std::string, NamedPropertyBase* > m_properties;
	};
}

#endif