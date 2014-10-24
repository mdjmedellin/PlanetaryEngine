#include "BSPNode2.hpp"


BSPNode2::BSPNode2()
	:	slice( Plane2() )
	,	rightChild( nullptr )
	,	leftChild( nullptr )
{}

BSPNode2::BSPNode2( const Plane2& slice, BSPNode2* rightChild, BSPNode2* leftChild )
	:	slice( slice )
	,	rightChild( rightChild )
	,	leftChild( leftChild )
{}

BSPNode2::BSPNode2( const BSPNode2& toCopy )
	:	slice( toCopy.slice )
	,	rightChild( toCopy.rightChild )
	,	leftChild( toCopy.leftChild )
{}

BSPNode2::~BSPNode2()
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
