#include "BSPNode3.hpp"


BSPNode3::BSPNode3()
	:	slice( Plane3() )
	,	rightChild( nullptr )
	,	leftChild( nullptr )
{}

BSPNode3::BSPNode3( const Plane3& slice, BSPNode3* rightChild, BSPNode3* leftChild )
	:	slice( slice )
	,	rightChild( rightChild )
	,	leftChild( leftChild )
{}

BSPNode3::BSPNode3( const BSPNode3& toCopy )
	:	slice( toCopy.slice )
	,	rightChild( toCopy.rightChild )
	,	leftChild( toCopy.leftChild )
{}

BSPNode3::~BSPNode3()
{
	if( rightChild != nullptr )
	{
		delete rightChild;
		rightChild = nullptr;
	}
	if( leftChild != nullptr )
	{
		delete leftChild;
		leftChild = nullptr;
	}
}
