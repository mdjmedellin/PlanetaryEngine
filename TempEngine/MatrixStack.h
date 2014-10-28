#pragma once
#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

//includes
#include "Matrix4X4.h"
#include <vector>
//

namespace gh
{
	class MatrixStack
	{
	public:
		MatrixStack();
		void ClearStack();
		void PushMatrix();
		void PopMatrix();
		Matrix4X4 back() const;
		void RotateX( float degrees );
		void RotateY( float degrees );
		void RotateZ( float degrees );
		void Translate( const Vector3& translation );
		void Translate(const Vector2& translation);
		void LoadIdentity();
		void PushPerspective( float fovy, float aspect, float zNear, float zFar );
		void PushOrtho( float left, float right, float bottom, float top, float zNear, float zFar );
		const float* GetTopMatrixFV() const;

	private:
		std::vector< Matrix4X4 > m_stack;
	};
}

#endif