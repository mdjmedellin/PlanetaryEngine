#pragma once
#ifndef CAMERA_H
#define CAMERA_H

//includes
#include "Vector3.hpp"
#include "Matrix4X4.h"
#include <Windows.h>
//

namespace gh
{
	class Camera
	{
	public:

		void applyImpulse( const Vector3& i )
		{
			if( m_mass > 0 )
			{
				m_acceleration += i / m_mass;
			}
		}

		void applyGravity( const Vector3& g )
		{
			if( m_mass > 0 && !m_isGrounded )
			{
				m_acceleration += g;
			}
		}

		Camera( bool allowToRotate = true );
		Camera( const Vector3& cameraPosition, const Vector3& playerDimensions = Vector3( .8f, 1.8f, .8f ), bool allowToRotate = true );

		const Vector3& rotation() const;
		const Vector3& position() const;

		void setPosition( const Vector3& newPosition );
		void updateInput();
		void updateDynamics( double deltaTime );

		Matrix4X4 fromEulerAngles( float pitch, float yaw, float roll = 0.f );

		inline bool isKeyDown( int virtualKey )
		{
			return ( GetAsyncKeyState( virtualKey ) & 0x8000 ) == 0x8000;
		}

	private:

		// Angles in degrees.
		//
		Vector3 m_rotation;			// Pitch, yaw, roll.
		Vector3 m_cameraPosition;
		Vector3 m_lastPosition;
		Vector3 m_velocity;
		Vector3 m_cursorPos;
		Vector3 m_acceleration;
		float m_drag;
		float m_mass;
		bool m_isGrounded;
		Vector3 m_dimensions;
		size_t m_nUpdates;
		Vector3 m_lastNotificationNormal;
		bool m_allowRotation;
	};
}

#endif