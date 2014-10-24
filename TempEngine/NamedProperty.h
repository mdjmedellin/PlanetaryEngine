#pragma once
#ifndef NAMED_PROPERTY_H
#define NAMED_PROPERTY_H

#include "NamedPropertyBase.h"

namespace gh
{
	template < typename T_dataType >
	class NamedProperty : public NamedPropertyBase
	{
		T_dataType m_data;
		
		NamedProperty( const T_dataType& data )
			:	m_data( data )
		{}
		
		friend class NamedProperties;

		virtual NamedPropertyBase* clone() const
		{
			return new NamedProperty< T_dataType >( m_data );
		}
	};
}

#endif