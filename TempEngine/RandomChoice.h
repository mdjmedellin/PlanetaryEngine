#pragma once
#ifndef RANDOM_CHOICE_H
#define RANDOM_CHOICE_H

namespace gh
{
	template < typename T_dataType >
	class RandomChoice
	{
		std::vector< std::pair< T_dataType, float > > m_choices;
		float m_maxWeight;

	public:

		RandomChoice( const std::vector< T_dataType >& dataChoices, const std::vector< float >& weightOfChoices )
			:	m_maxWeight( 0.f )
			,	m_choices( std::vector< std::pair< T_dataType, float > >() )
		{
			if( dataChoices.size() != weightOfChoices.size() )
			{
				return;
			}
			else
			{
				for( int index = 0; index < dataChoices.size(); ++index )
				{
					m_maxWeight += weightOfChoices[index];
					m_choices.push_back( std::pair< T_dataType, float >( dataChoices[ index ], weightOfChoices[ index ] ) );
				}

				float inverseMaxWeight = 1.f / m_maxWeight;
				for( int index = 0; index < dataChoices.size(); ++index )
				{
					m_choices[index].second *= inverseMaxWeight;
				}
			}
		}

		bool RandomChoice::GetRandomChoice( T_dataType& out_Choice ) const
		{
			float randomNumber = RandZeroToOne();

			float accumulatedWeight = 0.f;
			for( int index = 0; index < m_choices.size(); ++index )
			{
				accumulatedWeight += m_choices[index].second;
				if( accumulatedWeight >= randomNumber )
				{
					out_Choice = m_choices[index].first;
					return true;
				}
			}

			return false;
		}
	};
}

#endif