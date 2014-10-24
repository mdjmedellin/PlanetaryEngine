#include "Camera.h"
#include "Matrix4X4.h"
#include "MathUtilities.hpp"

namespace gh
{
	Camera::Camera( bool allowToRotate )
		:	m_rotation( 0.f, 0.f, 0.f )
		,	m_cameraPosition( 0.f, 0.f, 10.f )
		,	m_lastPosition( m_cameraPosition )
		,	m_velocity( 0.f, 0.f, 0.f )
		,	m_cursorPos( 0.f, 0.f, 0.f )
		,	m_acceleration( 0.f, 0.f, 0.f )
		,	m_drag( 0.2f )
		,	m_mass( 1.0f )
		,	m_isGrounded( false )
		,	m_dimensions( Vector3( .6f, 1.5f, .6f ) )
		,	m_nUpdates( 0 )
		,	m_allowRotation( allowToRotate)
	{}

	Camera::Camera( const Vector3& cameraPosition, const Vector3& playerDimensions, bool allowToRotate )
		:	m_cameraPosition( cameraPosition )
		,	m_lastPosition( cameraPosition )
		,	m_rotation( 0.f, 0.f, 0.f )
		,	m_velocity( 0.f, 0.f, 0.f )
		,	m_cursorPos( 0.f, 0.f, 0.f )
		,	m_acceleration( 0.f, 0.f, 0.f )
		,	m_drag( 0.2f )
		,	m_mass( 1.0f )
		,	m_isGrounded( false )
		,	m_dimensions( playerDimensions )
		,	m_nUpdates( 0 )
		,	m_allowRotation( allowToRotate )
	{}

	const Vector3& Camera::rotation() const
	{
		return m_rotation;
	}

	void Camera::updateInput()
	{
		if( m_allowRotation )
		{
			//update rotation
			//
			RECT screenRect;
			GetWindowRect( GetDesktopWindow(), &screenRect );
			float MOUSE_CENTER_X = screenRect.right / 2.f;
			float MOUSE_CENTER_Y = screenRect.bottom / 2.f;

			const float MOUSE_YAW_POWER = 0.2f;
			const float MOUSE_PITCH_POWER = 0.2f;

			POINT mousePos;
			GetCursorPos( &mousePos );

			float mouseDeltaX( mousePos.x - MOUSE_CENTER_X );
			float mouseDeltaY( mousePos.y - MOUSE_CENTER_Y );

			// Update camera yaw.
			//
			m_rotation.y -= mouseDeltaX * MOUSE_YAW_POWER;

			// Update camera pitch.
			//
			m_rotation.x -= mouseDeltaY * MOUSE_PITCH_POWER;
			m_rotation.x = ClampFloatWithinRange( -90.f, 90.f, m_rotation.x );

			SetCursorPos( (int) MOUSE_CENTER_X, (int) MOUSE_CENTER_Y );
		}

		//Update translation
		//
		float KEYBOARD_IMPULSE_POWER;
		if( isKeyDown( VK_SHIFT ) )
		{
			KEYBOARD_IMPULSE_POWER = .25f;
		}
		else
		{
			KEYBOARD_IMPULSE_POWER = .02f;
		}

		Vector3 inputImpulse;

		// Forward/backward
		if( isKeyDown( VK_UP ) || isKeyDown( 'W' ))
		{
			inputImpulse.z -= 1.0f;
		}
		if( isKeyDown( VK_DOWN ) || isKeyDown( 'S' ))
		{
			inputImpulse.z += 1.0f;
		}

		// left/right
		if( isKeyDown( VK_LEFT ) || isKeyDown( 'A' ))
		{
			inputImpulse.x -= 1.0f;
		}
		if( isKeyDown( VK_RIGHT ) || isKeyDown( 'D' ))
		{
			inputImpulse.x += 1.0f;
		}

		// Space bar moves up; Control and C keys move down.
		if( isKeyDown( VK_SPACE ) )
		{
			inputImpulse.y += 1.0f;
		}
		if( isKeyDown( VK_CONTROL ) || isKeyDown( 'C' ) )
		{
			inputImpulse.y -= 1.0f;
		}

		inputImpulse.normalize();
		inputImpulse *= KEYBOARD_IMPULSE_POWER;

		Matrix4X4 rotationMatrix = fromEulerAngles( m_rotation.x, m_rotation.y, 0.f );
		Vector3 worldSpaceImpulse = rotationMatrix.TransformPosition( inputImpulse );
		m_velocity += worldSpaceImpulse;
	}

	Matrix4X4 Camera::fromEulerAngles( float pitch, float yaw, float roll )
	{
		//in the Camera's perspective
		// +z is into the screen
		// +x is to the left of the screen
		// +y is up
		//
		
		//to the camera yaw is a rotation on its y axis
		Matrix4X4 kYMat = Matrix4X4::RotateYDegreesMatrix( yaw );

		//to the camera pitch is a rotation on its x axis
		Matrix4X4 kXMat = Matrix4X4::RotateXDegreesMatrix( pitch );

		//to the camera roll is a rotation on its z axis
		Matrix4X4 kZMat = Matrix4X4::RotateZDegreesMatrix( roll );
		
		return kYMat*(kXMat*kZMat);
	}

	const Vector3& Camera::position() const
	{
		return m_cameraPosition;
	}

	void Camera::setPosition( const Vector3& newPosition )
	{
		m_cameraPosition = newPosition;
	}

	void Camera::updateDynamics( double deltaTime )
	{
		++m_nUpdates;
		m_velocity -= m_velocity * m_drag;
		m_cameraPosition += m_velocity;
		m_cursorPos += m_velocity;

		//this delta time is for another implementation
		//one that was taken out and used for collision detection
		deltaTime = deltaTime;
	}
}