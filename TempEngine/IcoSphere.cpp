#include "IcoSphere.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Matrix4X4.h"
#include "Material.h"
#include "Light.h"
#include "MatrixStack.h"

namespace
{
	const float ICOSAHEDRON_CONST = .9510565163f;
}

namespace gh
{
	IcoSphere::IcoSphere( unsigned int maxRefinementLevel /* = 0 */, float radius /* = 1.f */ )
		:	m_sphereRadius( radius )
		,	m_maxRefinementLevel( maxRefinementLevel )
		,	m_refinementLevel( 0 )
		,	m_showOutline( false )
		,	m_idVBO( 0 )
		,	m_showNormals( false )
	{
		init();
	}

	void IcoSphere::init()
	{
		// IcoSphere initializes to an icosahedron
		// An icosahedron is made by 3 intersecting rectangles
		// http://en.wikipedia.org/wiki/Icosahedron

		float edgeSize = m_sphereRadius / ICOSAHEDRON_CONST;
		float halfEdge = edgeSize * .5f;
		float halfLongSideOfRect = sqrt( ( m_sphereRadius * m_sphereRadius ) - ( halfEdge * halfEdge ) );

		//build the vertices
		m_vertices.push_back( Vertex( Vector3(  halfEdge, 0.f, -halfLongSideOfRect ) ) );
		m_vertices.push_back( Vertex( Vector3( -halfEdge, 0.f, -halfLongSideOfRect ) ) );
		m_vertices.push_back( Vertex( Vector3(  halfEdge, 0.f,  halfLongSideOfRect ) ) );
		m_vertices.push_back( Vertex( Vector3( -halfEdge, 0.f,  halfLongSideOfRect ) ) );

		m_vertices.push_back( Vertex( Vector3( 0.f, -halfLongSideOfRect, -halfEdge ) ) );
		m_vertices.push_back( Vertex( Vector3( 0.f, -halfLongSideOfRect,  halfEdge ) ) );
		m_vertices.push_back( Vertex( Vector3( 0.f,  halfLongSideOfRect, -halfEdge ) ) );
		m_vertices.push_back( Vertex( Vector3( 0.f,  halfLongSideOfRect,  halfEdge ) ) );

		m_vertices.push_back( Vertex( Vector3( -halfLongSideOfRect, -halfEdge, 0.f ) ) );
		m_vertices.push_back( Vertex( Vector3(  halfLongSideOfRect, -halfEdge, 0.f ) ) );
		m_vertices.push_back( Vertex( Vector3( -halfLongSideOfRect,  halfEdge, 0.f ) ) );
		m_vertices.push_back( Vertex( Vector3(  halfLongSideOfRect,  halfEdge, 0.f ) ) );

		//build the triangles out of the vertices
		addTriangleFace( m_faces, Triangle( 7, 2, 11 ) );
		addTriangleFace( m_faces, Triangle( 7, 11, 6 ) );
		addTriangleFace( m_faces, Triangle( 7, 6, 10 ) );
		addTriangleFace( m_faces, Triangle( 7, 10, 3 ) );
		addTriangleFace( m_faces, Triangle( 7, 3, 2 ) );

		addTriangleFace( m_faces, Triangle( 11, 2, 9 ) );
		addTriangleFace( m_faces, Triangle( 2, 3, 5 ) );
		addTriangleFace( m_faces, Triangle( 3, 10, 8 ) );
		addTriangleFace( m_faces, Triangle( 10, 6, 1 ) );
		addTriangleFace( m_faces, Triangle( 6, 11, 0 ) );

		addTriangleFace( m_faces, Triangle( 4, 9, 5 ) );
		addTriangleFace( m_faces, Triangle( 4, 5, 8 ) );
		addTriangleFace( m_faces, Triangle( 4, 8, 1 ) );
		addTriangleFace( m_faces, Triangle( 4, 1, 0 ) );
		addTriangleFace( m_faces, Triangle( 4, 0, 9 ) );

		addTriangleFace( m_faces, Triangle( 9, 2, 5 ) );
		addTriangleFace( m_faces, Triangle( 5, 3, 8 ) );
		addTriangleFace( m_faces, Triangle( 8, 10, 1 ) );
		addTriangleFace( m_faces, Triangle( 1, 6, 0 ) );
		addTriangleFace( m_faces, Triangle( 0, 11, 9 ) );

		generateIndexList();
		generateIBOs();

		//refine the icoSphere
		while ( m_refinementLevel < m_maxRefinementLevel )
		{
			refineSphere();
			generateIndexList();
			generateIBOs();
		}

		generateVBO();
	}

	void IcoSphere::addTriangleFace( std::vector< Triangle >& faceContainer, Triangle faceToAdd )
	{
		m_vertices[ faceToAdd.p1 ].createSurfaceTangents( m_vertices[ faceToAdd.p3 ], m_vertices[ faceToAdd.p2 ] );
		m_vertices[ faceToAdd.p2 ].createSurfaceTangents( m_vertices[ faceToAdd.p1 ], m_vertices[ faceToAdd.p3 ] );
		m_vertices[ faceToAdd.p3 ].createSurfaceTangents( m_vertices[ faceToAdd.p2 ], m_vertices[ faceToAdd.p1 ] );

		faceContainer.push_back( faceToAdd );
	}

	void IcoSphere::draw( const Material& currentMaterial ) const
	{
		theRenderer.bindVBO( m_idVBO );

		if( currentMaterial.useShader() == 0 )
		{
			currentMaterial.bindTextures();

			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );

			glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, x ) ) );

			glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, u ) ) );

			theRenderer.drawTriangles( m_idTriangleIBOs[ m_refinementLevel ], triangle_indices[ m_refinementLevel ].size() );

			//tear down the vertex structure
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_VERTEX_ARRAY );

			currentMaterial.unbindTextures();
		}
		else
		{
			currentMaterial.useMaterial();

			theRenderer.drawTriangles( m_idTriangleIBOs[ m_refinementLevel ], triangle_indices[ m_refinementLevel ].size() );

			currentMaterial.removeMaterial();
		}

		if ( m_showNormals )
		{
			glUseProgram( 0 );

			glColor3f( 0.f, 0.f, 1.f );
			glLineWidth( 5.f );
			glBegin( GL_LINES );
			std::for_each( this->triangle_indices[ m_refinementLevel ].begin(),
							this->triangle_indices[ m_refinementLevel ].end(),
							[&]( unsigned int currentIndex )
			{
					Vertex currentVertex = m_vertices[ currentIndex ];
					Vector3 lineEnd = currentVertex.getPosition() + .1f * currentVertex.normal;
					glVertex3f( currentVertex.x, currentVertex.y, currentVertex.z );
					glVertex3f( lineEnd.x , lineEnd.y, lineEnd.z );
			});
			glEnd();

			glColor3f( 1.f, 0.f, 0.f );
			glBegin( GL_LINES );
			std::for_each( this->triangle_indices[ m_refinementLevel ].begin(),
							this->triangle_indices[ m_refinementLevel ].end(),
							[&]( unsigned int currentIndex )
			{
					Vertex currentVertex = m_vertices[ currentIndex ];
					Vector3 lineEnd = currentVertex.getPosition() + .1f * currentVertex.tangent;
					glVertex3f( currentVertex.x, currentVertex.y, currentVertex.z );
					glVertex3f( lineEnd.x , lineEnd.y, lineEnd.z );
			});
			glEnd();

			glColor3f( 0.f, 1.f, 0.f );
			glBegin( GL_LINES );
			std::for_each( this->triangle_indices[ m_refinementLevel ].begin(),
						this->triangle_indices[ m_refinementLevel ].end(),
						[&]( unsigned int currentIndex )
			{
				Vertex currentVertex = m_vertices[ currentIndex ];
				Vector3 lineEnd = currentVertex.getPosition() + .1f * currentVertex.bitangent;
				glVertex3f( currentVertex.x, currentVertex.y, currentVertex.z );
				glVertex3f( lineEnd.x , lineEnd.y, lineEnd.z );
			});
			glEnd();
		
			glLineWidth( 1.f);
		}
		glColor3f( 1.f, 1.f, 1.f );

		//Draw the outlines
		if( m_showOutline )
		{
			drawOutlines();
		}
	}

	void IcoSphere::drawOutlines() const
	{
		glUseProgram( 0 );
		glEnableClientState( GL_VERTEX_ARRAY );

		glDisable( GL_TEXTURE_2D );
		glLineWidth( 4.f );
		glColor3f( .1f, .1f ,.1f );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_idOutlineIBOs[ m_refinementLevel ] );
		glDrawElements( GL_LINES, outline_indices[ m_refinementLevel ].size(), GL_UNSIGNED_INT, 0 );
		glColor3f( 1, 1, 1 );
		glLineWidth( 1.f );

		//tear down the vertex structure
		glDisableClientState( GL_VERTEX_ARRAY );
	}

	void IcoSphere::refineSphere()
	{
		refineUp();

		auto faces2 = std::vector< Triangle >();
		size_t currentNumVertices = m_vertices.size();
		faces2.reserve( m_faces.size() );

		std::for_each( m_faces.begin(), m_faces.end(), [ & ]( const Triangle& currentTriangle ) 
		{
			// replace triangle by 4 triangles
			int a = getMiddlePointIndex( m_vertices[ currentTriangle.p1 ], m_vertices[ currentTriangle.p2 ], currentNumVertices );
			int b = getMiddlePointIndex( m_vertices[ currentTriangle.p2 ], m_vertices[ currentTriangle.p3 ], currentNumVertices );
			int c = getMiddlePointIndex( m_vertices[ currentTriangle.p3 ], m_vertices[ currentTriangle.p1 ], currentNumVertices );


			addTriangleFace( faces2, Triangle( currentTriangle.p1, a, c ) );
			addTriangleFace( faces2, Triangle( currentTriangle.p2, b, a ) );
			addTriangleFace( faces2, Triangle( currentTriangle.p3, c, b ) );
			addTriangleFace( faces2, Triangle( a, b, c ) );
		} );

		m_faces = faces2;
	}

	int IcoSphere::getMiddlePointIndex( const Vertex& p1, const Vertex& p2, int startFrom /*= 0*/ )
	{
		//interpolate between the points
		Vector3 newPoint = p1.getPosition() * .5f + p2.getPosition() * .5f;

		//set the length to that of the radius of the sphere
		newPoint.setRadialDistance( m_sphereRadius );
		Vertex toAdd( newPoint );
		return addVertex( toAdd, startFrom );
	}

	int IcoSphere::addVertex( const Vertex& toAdd, size_t startFrom /*= 0*/ )
	{
		//checking if there is not already a copy of the vertex in vertices
		for( auto currentVertex = m_vertices.begin() + startFrom; currentVertex != m_vertices.end(); ++currentVertex )
		{
			if( *( currentVertex ) == toAdd )
			{
				//we already have the vertex
				return ( currentVertex - m_vertices.begin() );
			}
		}

		//we did not have the vertex
		m_vertices.push_back( toAdd );
		return m_vertices.size() - 1;
	}

	void IcoSphere::generateIndexList()
	{
		std::vector< unsigned int > triangleIndices;
		std::vector< unsigned int > outlineIndices;

		std::for_each( m_faces.begin(), m_faces.end(), [ & ]( const Triangle& currentTriangle ) 
		{
			triangleIndices.push_back( currentTriangle.p1 );
			outlineIndices.push_back( currentTriangle.p1 );
			outlineIndices.push_back( currentTriangle.p2 );
			
			triangleIndices.push_back( currentTriangle.p2 );
			outlineIndices.push_back( currentTriangle.p2 );
			outlineIndices.push_back( currentTriangle.p3 );
			
			triangleIndices.push_back( currentTriangle.p3 );
			outlineIndices.push_back( currentTriangle.p3 );
			outlineIndices.push_back( currentTriangle.p1 );
		} );

		triangle_indices.push_back( triangleIndices );
		outline_indices.push_back( outlineIndices );
	}

	void IcoSphere::generateVBO()
	{
		theRenderer.overwriteVBO( m_idVBO, m_vertices.size() * sizeof( m_vertices.front() ), m_vertices.data() );
	}

	void IcoSphere::generateIBOs()
	{
		//Generate IBO of triangles
		unsigned int triangleIBO = 0;

		//add the IBO of triangles to the corresponding vector
		triangleIBO = theRenderer.generateIBO( triangle_indices[ m_refinementLevel ].size() * sizeof( triangle_indices[ m_refinementLevel ].front() ),
												triangle_indices[ m_refinementLevel ].data() );

		m_idTriangleIBOs.push_back( triangleIBO );

		//Generate IBO of outline
		unsigned int outlineIBO = 0;

		outlineIBO = theRenderer.generateIBO( outline_indices[ m_refinementLevel ].size() * sizeof( outline_indices[ m_refinementLevel ].front() ),
												outline_indices[ m_refinementLevel ].data() );

		//add the IBO of the outline to the corresponding vector
		m_idOutlineIBOs.push_back( outlineIBO );
	}

	void IcoSphere::refineUp()
	{
		m_refinementLevel = m_refinementLevel < m_maxRefinementLevel ? m_refinementLevel + 1 : m_refinementLevel;
	}

	void IcoSphere::refineDown()
	{
		m_refinementLevel = m_refinementLevel > 0 ? m_refinementLevel - 1 : m_refinementLevel;
	}

	void IcoSphere::toggleOutline()
	{
		m_showOutline = !m_showOutline;
	}

	void IcoSphere::toggleNormals()
	{
		m_showNormals = !m_showNormals;
	}

}