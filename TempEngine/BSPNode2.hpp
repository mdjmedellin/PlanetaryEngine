#pragma once
#ifndef BSPNODE2_H
#define BSPNODE2_H

#include "Plane2.hpp"

class BSPNode2
{
public:
	BSPNode2();
	explicit BSPNode2( const Plane2& slice, BSPNode2* rightChild, BSPNode2* leftChild );
	BSPNode2( const BSPNode2& toCopy );
	~BSPNode2();

protected:
	Plane2 slice;
	BSPNode2* rightChild;
	BSPNode2* leftChild;
};

#endif