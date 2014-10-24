#pragma once
#ifndef BSPNODE3_H
#define BSPNODE3_H

#include "Plane3.hpp"

class BSPNode3
{
public:
	BSPNode3();
	explicit BSPNode3( const Plane3& slice, BSPNode3* rightChild, BSPNode3* leftChild );
	BSPNode3( const BSPNode3& toCopy );
	~BSPNode3();

protected:
	Plane3 slice;
	BSPNode3* rightChild;
	BSPNode3* leftChild;
};

#endif