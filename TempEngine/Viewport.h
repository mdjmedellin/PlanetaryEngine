#pragma once
#ifndef VIEWPORT_H
#define VIEWPORT_H

class Vector3;
class Vector4;

namespace gh
{
	class Viewport
	{
	public:
		Viewport(float minX, float minY, float maxX, float maxY);
		void SetViewport(float minX, float minY, float maxX, float maxY);
		void GetNormalizedDeviceCoordinates(Vector4& out_ndc, const Vector3& screenCoordinates);

	private:
		float m_viewportValues[4];
	};
}

#endif