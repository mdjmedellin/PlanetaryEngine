#pragma once
#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include <vector>
#include <algorithm>
#include "MathUtilities.hpp"
#include "MatrixStack.h"

class Material;
class PointLight;

namespace gh
{
	class Texture;

	struct Triangle
	{
		int p1, p2, p3;

		Triangle( int p1, int p2, int p3 )
			:	p1( p1 )
			,	p2( p2 )
			,	p3( p3 )
		{}
	};


	class IcoSphere
	{
	public:
		IcoSphere( unsigned int maxRefinementLevel = 0, float radius = 1.f );
		void draw( const Material& currentMaterial ) const;
		void refineUp();
		void refineDown();
		void toggleOutline();
		void toggleNormals();

	private:
		void init();
		void drawOutlines() const;
		int getMiddlePointIndex( const Vertex& p1, const Vertex& p2, int startFrom = 0 );
		int addVertex( const Vertex& toAdd, size_t startFrom = 0 );
		void generateIndexList();
		void generateVBO();
		void refineSphere();
		void generateIBOs();
		void addTriangleFace( std::vector< Triangle >& faceContainer, Triangle faceToAdd );
		
		std::vector< Triangle > m_faces;
		std::vector< Vertex > m_vertices;
		std::vector< std::vector< unsigned int > > triangle_indices;
		std::vector< std::vector< unsigned int > > outline_indices;
		std::vector< unsigned int > m_idTriangleIBOs, m_idOutlineIBOs;
		unsigned int m_idVBO;
		unsigned int m_refinementLevel;
		unsigned int m_maxRefinementLevel;
		float m_sphereRadius;
		bool m_showOutline;
		bool m_showNormals;
	};
}

#endif