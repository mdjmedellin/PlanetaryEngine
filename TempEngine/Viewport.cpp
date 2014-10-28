#include "Viewport.h"
#include "Vector3.hpp"
#include "Vector4.h"

namespace gh
{
	Viewport::Viewport( float minX, float minY, float maxX, float maxY )
	{
		m_viewportValues[0] = minX;
		m_viewportValues[1] = minY;
		m_viewportValues[2] = maxX;
		m_viewportValues[3] = maxY;
	}

	void Viewport::SetViewport( float minX, float minY, float maxX, float maxY )
	{
		m_viewportValues[0] = minX;
		m_viewportValues[1] = minY;
		m_viewportValues[2] = maxX;
		m_viewportValues[3] = maxY;
	}

	void Viewport::GetNormalizedDeviceCoordinates( Vector4& out_ndc, const Vector3& screenCoordinates )
	{
		out_ndc.x = ( ( (2 * (screenCoordinates.x - m_viewportValues[0]) ) / m_viewportValues[2] ) - 1 );
		out_ndc.y = ( ( (2 * (screenCoordinates.y - m_viewportValues[1]) ) / m_viewportValues[3] ) - 1 );
		out_ndc.z = ( 2 * screenCoordinates.z ) - 1;
		out_ndc.w = 1;
	}
}