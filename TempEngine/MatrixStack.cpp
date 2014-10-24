
#include "MatrixStack.h"

namespace gh
{
	MatrixStack::MatrixStack()
	{
		//initialize by pushing the identity matrix into the stack
		m_stack.push_back( Matrix4X4() );
	}

	void MatrixStack::ClearStack()
	{
		m_stack.clear();
		m_stack.push_back( Matrix4X4() );
	}

	void MatrixStack::PushMatrix()
	{
		//create copy
		Matrix4X4 toAdd = m_stack.back();
		m_stack.push_back( toAdd );
	}

	void MatrixStack::PopMatrix()
	{
		//check that the matrix is not empty or does not only contain 1 matrix
		if( m_stack.size() > 1 )
		{
			m_stack.pop_back();
		}
		else
		{
			m_stack.back().MakeIdentity();
		}
	}

	void MatrixStack::RotateX( float degrees )
	{
		Matrix4X4 rotationMatrix = Matrix4X4::RotateXDegreesMatrix( degrees );
		m_stack.back() = m_stack.back() * rotationMatrix;
	}

	void MatrixStack::RotateY( float degrees )
	{
		Matrix4X4 rotationMatrix = Matrix4X4::RotateYDegreesMatrix( degrees );
		m_stack.back() = m_stack.back() * rotationMatrix;
	}

	void MatrixStack::RotateZ( float degrees )
	{
		Matrix4X4 rotationMatrix = Matrix4X4::RotateZDegreesMatrix( degrees );
		m_stack.back() = m_stack.back() * rotationMatrix;
	}

	void MatrixStack::Translate( const Vector3& translation )
	{
		Matrix4X4 translationMatrix = Matrix4X4::TranslationMatrix( translation );
		m_stack.back() = m_stack.back() * translationMatrix;
	}

	void MatrixStack::LoadIdentity()
	{
		m_stack.back().MakeIdentity();
	}

	void MatrixStack::PushPerspective( float fovy, float aspect, float zNear, float zFar )
	{
		Matrix4X4 perspectiveMatrix = Matrix4X4::GetPerspectiveMatrix( fovy, aspect, zNear, zFar );

		m_stack.back() = m_stack.back() * perspectiveMatrix;
	}

	void MatrixStack::PushOrtho( float left, float right, float bottom, float top, float zNear, float zFar )
	{
		Matrix4X4 orthoMatrix = Matrix4X4::GetOrthoMatrix( left, right, bottom, top, zNear, zFar );

		m_stack.back() = orthoMatrix;
	}

	const float* MatrixStack::GetTopMatrixFV() const
	{
		return &( m_stack.back().m_values[ 0 ] );
	}

	Matrix4X4 MatrixStack::back() const
	{
		return m_stack.back();
	}

}