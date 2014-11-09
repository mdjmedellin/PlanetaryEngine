#include "Matrix4X4.h"
#include "MathUtilities.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.h"
#include <iostream>
#include <math.h>

Matrix4X4::Matrix4X4()
{
	for( int index = 0; index < 16; ++index)
	{
		m_values[ index ] = 0;
	}

	m_values[ 0 ] = 1;
	m_values[ 5 ] = 1;
	m_values[ 10 ] = 1;
	m_values[ 15 ] = 1;
}

Matrix4X4::Matrix4X4( const Matrix4X4& toCopy )
{
	for ( int index = 0; index < 16; ++index )
	{
		m_values[ index ] = toCopy.m_values[ index ];
	}
}


Matrix4X4::~Matrix4X4()
{}

//rotate z
Matrix4X4 Matrix4X4::RotateZDegreesMatrix( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );

	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 0 ] = c;
	toReturn.m_values[ 1 ] = s;
	toReturn.m_values[ 4 ] = -s;
	toReturn.m_values[ 5 ] = c;

	return toReturn;
}

Matrix4X4 Matrix4X4::RotateZRadiansMatrix( float radians )
{
	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 0 ] = c;
	toReturn.m_values[ 1 ] = s;
	toReturn.m_values[ 4 ] = -s;
	toReturn.m_values[ 5 ] = c;

	return toReturn;

}

//rotate y
Matrix4X4 Matrix4X4::RotateYDegreesMatrix( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );

	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 0 ] = c;
	toReturn.m_values[ 2 ] = -s;
	toReturn.m_values[ 8 ] = s;
	toReturn.m_values[ 10 ] = c;

	return toReturn;

}

//These are all column major
Matrix4X4 Matrix4X4::RotateYRadiansMatrix( float radians )
{
	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 0 ] = c;
	toReturn.m_values[ 2 ] = -s;
	toReturn.m_values[ 8 ] = s;
	toReturn.m_values[ 10 ] = c;

	return toReturn;
}

//rotate x
Matrix4X4 Matrix4X4::RotateXDegreesMatrix( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );

	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 5 ] = c;
	toReturn.m_values[ 6 ] = s;
	toReturn.m_values[ 9 ] = -s;
	toReturn.m_values[ 10 ] = c;

	return toReturn;
}

Matrix4X4 Matrix4X4::RotateXRadiansMatrix( float radians )
{
	Matrix4X4 toReturn;

	float c = cos( radians );
	float s = sin( radians );

	toReturn.m_values[ 5 ] = c;
	toReturn.m_values[ 6 ] = s;
	toReturn.m_values[ 9 ] = -s;
	toReturn.m_values[ 10 ] = c;

	return toReturn;
}

Matrix4X4 Matrix4X4::MatrixFromEulerAngles( float pitch, float yaw, float roll )
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

void Matrix4X4::MakeIdentity()
{
	for( int index = 0; index < 16; ++index )
	{
		m_values[ index ] = 0;
	}

	m_values[ 0 ] = 1;
	m_values[ 5 ] = 1;
	m_values[ 10 ] = 1;
	m_values[ 15 ] = 1;
}

void Matrix4X4::operator=( const Matrix4X4& toCopy )
{
	for( int index = 0; index < 16; ++index )
	{
		m_values[ index ] = toCopy.m_values[ index ];
	}
}

Matrix4X4 Matrix4X4::operator*( const Matrix4X4& rightHandSide )
{
	Matrix4X4 toReturn;

	//multiplying the matrices
	toReturn.m_values[ 0 ] = m_values [ 0 ] * rightHandSide.m_values[ 0 ] + m_values [ 4 ] * rightHandSide.m_values[ 1 ]
			+ m_values [ 8 ] * rightHandSide.m_values[ 2 ] + m_values [ 12 ] * rightHandSide.m_values[ 3 ];
	
	toReturn.m_values[ 1 ] = m_values [ 1 ] * rightHandSide.m_values[ 0 ] + m_values [ 5 ] * rightHandSide.m_values[ 1 ]
			+ m_values [ 9 ] * rightHandSide.m_values[ 2 ] + m_values [ 13 ] * rightHandSide.m_values[ 3 ];
	
	toReturn.m_values[ 2 ] = m_values [ 2 ] * rightHandSide.m_values[ 0 ] + m_values [ 6 ] * rightHandSide.m_values[ 1 ]
			+ m_values [ 10 ] * rightHandSide.m_values[ 2 ] + m_values [ 14 ] * rightHandSide.m_values[ 3 ];
	
	toReturn.m_values[ 3 ] = m_values [ 3 ] * rightHandSide.m_values[ 0 ] + m_values [ 7 ] * rightHandSide.m_values[ 1 ]
			+ m_values [ 11 ] * rightHandSide.m_values[ 2 ] + m_values [ 15 ] * rightHandSide.m_values[ 3 ];
	
	toReturn.m_values[ 4 ] = m_values [ 0 ] * rightHandSide.m_values[ 4 ] + m_values [ 4 ] * rightHandSide.m_values[ 5 ]
			+ m_values [ 8 ] * rightHandSide.m_values[ 6 ] + m_values [ 12 ] * rightHandSide.m_values[ 7 ];
	
	toReturn.m_values[ 5 ] = m_values [ 1 ] * rightHandSide.m_values[ 4 ] + m_values [ 5 ] * rightHandSide.m_values[ 5 ]
			+ m_values [ 9 ] * rightHandSide.m_values[ 6 ] + m_values [ 13 ] * rightHandSide.m_values[ 7 ];
	
	toReturn.m_values[ 6 ] = m_values [ 2 ] * rightHandSide.m_values[ 4 ] + m_values [ 6 ] * rightHandSide.m_values[ 5 ]
			+ m_values [ 10 ] * rightHandSide.m_values[ 6 ] + m_values [ 14 ] * rightHandSide.m_values[ 7 ];
	
	toReturn.m_values[ 7 ] = m_values [ 3 ] * rightHandSide.m_values[ 4 ] + m_values [ 7 ] * rightHandSide.m_values[ 5 ]
			+ m_values [ 11 ] * rightHandSide.m_values[ 6 ] + m_values [ 15 ] * rightHandSide.m_values[ 7 ];
			
	toReturn.m_values[ 8 ] = m_values [ 0 ] * rightHandSide.m_values[ 8 ] + m_values [ 4 ] * rightHandSide.m_values[ 9 ]
			+ m_values [ 8 ] * rightHandSide.m_values[ 10 ] + m_values [ 12 ] * rightHandSide.m_values[ 11 ];
	
	toReturn.m_values[ 9 ] = m_values [ 1 ] * rightHandSide.m_values[ 8 ] + m_values [ 5 ] * rightHandSide.m_values[ 9 ]
			+ m_values [ 9 ] * rightHandSide.m_values[ 10 ] + m_values [ 13 ] * rightHandSide.m_values[ 11 ];
	
	toReturn.m_values[ 10 ] = m_values [ 2 ] * rightHandSide.m_values[ 8 ] + m_values [ 6 ] * rightHandSide.m_values[ 9 ]
			+ m_values [ 10 ] * rightHandSide.m_values[ 10 ] + m_values [ 14 ] * rightHandSide.m_values[ 11 ];
	
	toReturn.m_values[ 11 ] = m_values [ 3 ] * rightHandSide.m_values[ 8 ] + m_values [ 7 ] * rightHandSide.m_values[ 9 ]
			+ m_values [ 11 ] * rightHandSide.m_values[ 10 ] + m_values [ 15 ] * rightHandSide.m_values[ 11 ];
	
	toReturn.m_values[ 12 ] = m_values [ 0 ] * rightHandSide.m_values[ 12 ] + m_values [ 4 ] * rightHandSide.m_values[ 13 ]
			+ m_values [ 8 ] * rightHandSide.m_values[ 14 ] + m_values [ 12 ] * rightHandSide.m_values[ 15 ];
	
	toReturn.m_values[ 13 ] = m_values [ 1 ] * rightHandSide.m_values[ 12 ] + m_values [ 5 ] * rightHandSide.m_values[ 13 ]
			+ m_values [ 9 ] * rightHandSide.m_values[ 14 ] + m_values [ 13 ] * rightHandSide.m_values[ 15 ];
	
	toReturn.m_values[ 14 ] = m_values [ 2 ] * rightHandSide.m_values[ 12 ] + m_values [ 6 ] * rightHandSide.m_values[ 13 ]
			+ m_values [ 10 ] * rightHandSide.m_values[ 14 ] + m_values [ 14 ] * rightHandSide.m_values[ 15 ];
	
	toReturn.m_values[ 15 ] = m_values [ 3 ] * rightHandSide.m_values[ 12 ] + m_values [ 7 ] * rightHandSide.m_values[ 13 ]
					+ m_values [ 11 ] * rightHandSide.m_values[ 14 ] + m_values [ 15 ] * rightHandSide.m_values[ 15 ];

	return toReturn;
}

Matrix4X4 Matrix4X4::NonUniformScaleMatrix( const Vector2& scale )
{
	Matrix4X4 toReturn;

	toReturn.m_values[ 0 ] = scale.x;
	toReturn.m_values[ 5 ] = scale.y;
	
	return toReturn;
}

Matrix4X4 Matrix4X4::NonUniformScaleMatrix( const Vector3& scale )
{
	Matrix4X4 toReturn;

	toReturn.m_values[ 0 ] = scale.x;
	toReturn.m_values[ 5 ] = scale.y;
	toReturn.m_values[ 10 ] = scale.z;

	return toReturn;
}

Matrix4X4 Matrix4X4::TranslationMatrix( const Vector2& translation )
{
	Matrix4X4 toReturn;

	toReturn.m_values[ 12 ] = translation.x;
	toReturn.m_values[ 13 ] = translation.y;

	return toReturn;
}

Matrix4X4 Matrix4X4::TranslationMatrix( const Vector3& translation )
{
	Matrix4X4 toReturn;

	toReturn.m_values[ 12 ] = translation.x;
	toReturn.m_values[ 13 ] = translation.y;
	toReturn.m_values[ 14 ] = translation.z;

	return toReturn;
}

Vector4 Matrix4X4::TransformPoint( const Vector4& point ) const
{
	float x = point.x * m_values [ 0 ] + point.y * m_values [ 4 ] + point.z * m_values [ 8 ] + point.w * m_values [ 12 ];
	float y = point.x * m_values [ 1 ] + point.y * m_values [ 5 ] + point.z * m_values [ 9 ] + point.w * m_values [ 13 ];
	float z = point.x * m_values [ 2 ] + point.y * m_values [ 6 ] + point.z * m_values [ 10 ] + point.w * m_values [ 14 ];
	float w = point.x * m_values [ 3 ] + point.y * m_values [ 7 ] + point.z * m_values [ 11 ] + point.w * m_values [ 15 ];

	return Vector4( x, y, z, w );
}

Vector3 Matrix4X4::TransformPosition( const Vector3& position ) const
{
	float x = position.x * m_values [ 0 ] + position.y * m_values [ 4 ] + position.z * m_values [ 8 ] + m_values [ 12 ];
	float y = position.x * m_values [ 1 ] + position.y * m_values [ 5 ] + position.z * m_values [ 9 ] + m_values [ 13 ];
	float z = position.x * m_values [ 2 ] + position.y * m_values [ 6 ] + position.z * m_values [ 10 ] + m_values [ 14 ];

	return Vector3( x, y, z );
}

Vector2 Matrix4X4::TransformPosition( const Vector2& position ) const
{
	float x = position.x * m_values [ 0 ] + position.y * m_values [ 4 ] + m_values [ 12 ];
	float y = position.x * m_values [ 1 ] + position.y * m_values [ 5 ] + m_values [ 13 ];

	return Vector2( x, y );
}

Vector3 Matrix4X4::TransformDirection( const Vector3& direction ) const
{
	float x = direction.x * m_values [ 0 ] + direction.y * m_values [ 4 ] + direction.z * m_values [ 8 ];
	float y = direction.x * m_values [ 1 ] + direction.y * m_values [ 5 ] + direction.z * m_values [ 9 ];
	float z = direction.x * m_values [ 2 ] + direction.y * m_values [ 6 ] + direction.z * m_values [ 10 ];

	return Vector3( x, y, z );
}

Vector2 Matrix4X4::TransformDirection( const Vector2& direction ) const
{
	float x = direction.x * m_values [ 0 ] + direction.y * m_values [ 4 ];
	float y = direction.x * m_values [ 1 ] + direction.y * m_values [ 5 ];

	return Vector2( x, y );
}

Vector3 Matrix4X4::GetIBasis() const
{
	return Vector3( m_values[ 0 ], m_values[ 1 ], m_values[ 2 ] );
}

Vector3 Matrix4X4::GetJBasis() const
{
	return Vector3( m_values[ 4 ], m_values[ 5 ], m_values[ 6 ] );
}

Vector3 Matrix4X4::GetKBasis() const
{
	return Vector3( m_values[ 8 ], m_values[ 9 ], m_values[ 10 ] );
}

Vector3 Matrix4X4::GetTranslationBasis() const
{
	return Vector3( m_values[ 12 ], m_values[ 13 ], m_values[ 14 ] );
}

void Matrix4X4::printMatrix() const
{
	for( int i = 0; i < 4; ++i )
	{
		std::cout<< m_values[0+i]<<"\t"<<m_values[4+i]<<"\t"<<m_values[8+i]<<"\t"<<m_values[12+i]<<std::endl; 
	}
}

Matrix4X4 Matrix4X4::GetPerspectiveMatrix( float fovy, float aspect, float zNear, float zFar )
{
	float f = ( 1.f / ( tan( ConvertDegreesToRadians( fovy / 2.f ) ) ) );

	Matrix4X4 toReturn;

	toReturn.m_values[ 0 ] = ( f / aspect );
	toReturn.m_values[ 5 ] = ( f );
	toReturn.m_values[ 10 ] = ( ( zFar + zNear ) / ( zNear - zFar ) );
	toReturn.m_values[ 11 ] = -1.f;
	toReturn.m_values[ 14 ] = ( ( 2.f * zFar * zNear ) / ( zNear - zFar ) );
	toReturn.m_values[ 15 ] = 0.f;

	return toReturn;
}

Matrix4X4 Matrix4X4::GetOrthoMatrix( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Matrix4X4 toReturn;

	float tx = -( ( right + left ) / ( right - left ) );
	float ty = -( ( top + bottom ) / ( top - bottom ) );
	float tz = -( ( zFar + zNear ) / ( zFar - zNear ) );

	toReturn.m_values[ 0 ] = ( 2.f / ( right - left ) );
	toReturn.m_values[ 5 ] = ( 2.f / ( top - bottom ) );
	toReturn.m_values[ 10 ] = ( -2.f / ( zFar - zNear ) );
	toReturn.m_values[ 12 ] = tx;
	toReturn.m_values[ 13 ] = ty;
	toReturn.m_values[ 14 ] = tz;
	toReturn.m_values[ 15 ] = 1.f;

	return toReturn;
}

Matrix4X4 Matrix4X4::LookAtMatrix( const Vector3& sourcePosition, const Vector3& targetPosition )
{
	Vector3 i( 1.f, 0.f, 0.f );
	Vector3 j( 0.f, 1.f, 0.f );
	Vector3 k( sourcePosition.x - targetPosition.x, sourcePosition.y - targetPosition.y, sourcePosition.z - targetPosition.z );
	k.normalize();
	if( k != Vector3( 0.0f, 1.0f, 0.0f ) && k != Vector3( 0.f, -1.f, 0.f ) )
	{
		i = j.CrossProduct( k );
		j = k.CrossProduct( i );
	}
	else
	{
		j = k.CrossProduct( i );
		i = j.CrossProduct( k );
	}

	Matrix4X4 rc;
	rc.SetIBasis( i );
	rc.SetJBasis( j );
	rc.SetKBasis( k );

	return rc;
}

void Matrix4X4::SetIBasis( const Vector3& iBasis )
{
	m_values[ 0 ] = iBasis.x;
	m_values[ 1 ] = iBasis.y;
	m_values[ 2 ] = iBasis.z;
}

void Matrix4X4::SetJBasis( const Vector3& jBasis )
{
	m_values[ 4 ] = jBasis.x;
	m_values[ 5 ] = jBasis.y;
	m_values[ 6 ] = jBasis.z;
}

void Matrix4X4::SetKBasis( const Vector3& kBasis )
{
	m_values[ 8 ] = kBasis.x;
	m_values[ 9 ] = kBasis.y;
	m_values[ 10 ] = kBasis.z;
}

bool Matrix4X4::InvertMatrix()
{
	float inv[16];
	float determinant = 0.f;

	//calculate the determinant
	inv[0] =	m_values[5]  * m_values[10] * m_values[15] - 
				m_values[5]  * m_values[11] * m_values[14] - 
				m_values[9]  * m_values[6]  * m_values[15] + 
				m_values[9]  * m_values[7]  * m_values[14] +
				m_values[13] * m_values[6]  * m_values[11] - 
				m_values[13] * m_values[7]  * m_values[10];

	inv[4] =	-m_values[4]  * m_values[10] * m_values[15] + 
				 m_values[4]  * m_values[11] * m_values[14] + 
				 m_values[8]  * m_values[6]  * m_values[15] - 
				 m_values[8]  * m_values[7]  * m_values[14] - 
				 m_values[12] * m_values[6]  * m_values[11] + 
				 m_values[12] * m_values[7]  * m_values[10];

	inv[8] =	m_values[4]  * m_values[9]	* m_values[15] - 
				m_values[4]  * m_values[11] * m_values[13] - 
				m_values[8]  * m_values[5]	* m_values[15] + 
				m_values[8]  * m_values[7]	* m_values[13] + 
				m_values[12] * m_values[5]	* m_values[11] - 
				m_values[12] * m_values[7]	* m_values[9];

	inv[12] =	-m_values[4]  * m_values[9]  * m_values[14] + 
				 m_values[4]  * m_values[10] * m_values[13] +
				 m_values[8]  * m_values[5]  * m_values[14] - 
				 m_values[8]  * m_values[6]  * m_values[13] - 
				 m_values[12] * m_values[5]  * m_values[10] + 
				 m_values[12] * m_values[6]  * m_values[9];

	inv[1] =	-m_values[1]  * m_values[10] * m_values[15] + 
				 m_values[1]  * m_values[11] * m_values[14] + 
				 m_values[9]  * m_values[2]  * m_values[15] - 
				 m_values[9]  * m_values[3]  * m_values[14] - 
				 m_values[13] * m_values[2]  * m_values[11] + 
				 m_values[13] * m_values[3]  * m_values[10];

	inv[5] =	m_values[0]  * m_values[10] * m_values[15] - 
				m_values[0]  * m_values[11] * m_values[14] - 
				m_values[8]  * m_values[2]  * m_values[15] + 
				m_values[8]  * m_values[3]  * m_values[14] + 
				m_values[12] * m_values[2]  * m_values[11] - 
				m_values[12] * m_values[3]  * m_values[10];

	inv[9] =	-m_values[0]  * m_values[9]  * m_values[15] + 
				 m_values[0]  * m_values[11] * m_values[13] + 
				 m_values[8]  * m_values[1]  * m_values[15] - 
				 m_values[8]  * m_values[3]  * m_values[13] - 
				 m_values[12] * m_values[1]  * m_values[11] + 
				 m_values[12] * m_values[3]  * m_values[9];

	inv[13] =	m_values [0]  * m_values[9]  * m_values[14] - 
				m_values [0]  * m_values[10] * m_values[13] - 
				m_values [8]  * m_values[1]  * m_values[14] + 
				m_values [8]  * m_values[2]  * m_values[13] + 
				m_values [12] * m_values[1]  * m_values[10] - 
				m_values [12] * m_values[2]  * m_values[9];

	inv[2] =	m_values[1]  * m_values[6] * m_values[15] - 
				m_values[1]  * m_values[7] * m_values[14] - 
				m_values[5]  * m_values[2] * m_values[15] + 
				m_values[5]  * m_values[3] * m_values[14] + 
				m_values[13] * m_values[2] * m_values[7] - 
				m_values[13] * m_values[3] * m_values[6];

	inv[6] =	-m_values[0]  * m_values[6] * m_values[15] + 
				 m_values[0]  * m_values[7] * m_values[14] + 
				 m_values[4]  * m_values[2] * m_values[15] - 
				 m_values[4]  * m_values[3] * m_values[14] - 
				 m_values[12] * m_values[2] * m_values[7] + 
				 m_values[12] * m_values[3] * m_values[6];

	inv[10] =	m_values[0]  * m_values[5] * m_values[15] - 
				m_values[0]  * m_values[7] * m_values[13] - 
				m_values[4]  * m_values[1] * m_values[15] + 
				m_values[4]  * m_values[3] * m_values[13] + 
				m_values[12] * m_values[1] * m_values[7] - 
				m_values[12] * m_values[3] * m_values[5];

	inv[14] =	-m_values[0]  * m_values[5] * m_values[14] + 
				 m_values[0]  * m_values[6] * m_values[13] + 
				 m_values[4]  * m_values[1] * m_values[14] - 
				 m_values[4]  * m_values[2] * m_values[13] - 
				 m_values[12] * m_values[1] * m_values[6] + 
				 m_values[12] * m_values[2] * m_values[5];

	inv[3] =	-m_values[1] * m_values[6] * m_values[11] + 
				 m_values[1] * m_values[7] * m_values[10] + 
				 m_values[5] * m_values[2] * m_values[11] - 
				 m_values[5] * m_values[3] * m_values[10] - 
				 m_values[9] * m_values[2] * m_values[7] + 
				 m_values[9] * m_values[3] * m_values[6];

	inv[7] =	m_values[0] * m_values[6] * m_values[11] - 
				m_values[0] * m_values[7] * m_values[10] - 
				m_values[4] * m_values[2] * m_values[11] + 
				m_values[4] * m_values[3] * m_values[10] + 
				m_values[8] * m_values[2] * m_values[7] - 
				m_values[8] * m_values[3] * m_values[6];

	inv[11] =	-m_values[0] * m_values[5] * m_values[11] + 
				 m_values[0] * m_values[7] * m_values[9] + 
				 m_values[4] * m_values[1] * m_values[11] - 
				 m_values[4] * m_values[3] * m_values[9] - 
				 m_values[8] * m_values[1] * m_values[7] + 
				 m_values[8] * m_values[3] * m_values[5];

	inv[15] =	m_values[0] * m_values[5] * m_values[10] - 
				m_values[0] * m_values[6] * m_values[9] - 
				m_values[4] * m_values[1] * m_values[10] + 
				m_values[4] * m_values[2] * m_values[9] + 
				m_values[8] * m_values[1] * m_values[6] - 
				m_values[8] * m_values[2] * m_values[5];

	determinant = m_values[0] * inv[0] + m_values[1] * inv[4] + m_values[2] * inv[8] + m_values[3] * inv[12];

	if (determinant == 0)
		return false;

	determinant = 1.0 / determinant;

	for (int i = 0; i < 16; i++)
		m_values[i] = inv[i] * determinant;

	return true;
}
