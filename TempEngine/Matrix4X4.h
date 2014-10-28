#pragma once
#ifndef MATRIX4X4
#define MATRIX4X4

class Vector2;
class Vector3;
class Vector4;

//COLUMN MAJOR?

class Matrix4X4
{
public:
	Matrix4X4();
	Matrix4X4( const Matrix4X4& toCopy );
	~Matrix4X4();

	//Rotation Matrices
	//
	static Matrix4X4 RotateXDegreesMatrix( float degrees );
	static Matrix4X4 RotateXRadiansMatrix( float radians );
	static Matrix4X4 RotateYDegreesMatrix( float degrees );
	static Matrix4X4 RotateYRadiansMatrix( float radians );
	static Matrix4X4 RotateZDegreesMatrix( float degrees );
	static Matrix4X4 RotateZRadiansMatrix( float radians );

	//perspective
	static Matrix4X4 GetPerspectiveMatrix( float fovy, float aspect, float zNear, float zFar );

	//ortho
	static Matrix4X4 GetOrthoMatrix( float left, float right, float bottom, float top, float zNear, float zFar );

	//function taken from camera operations
	static Matrix4X4 MatrixFromEulerAngles( float pitch, float yaw, float roll );

	//Scale
	//
	static Matrix4X4 NonUniformScaleMatrix( const Vector2& scale );
	static Matrix4X4 NonUniformScaleMatrix( const Vector3& scale );

	//Translation
	//
	static Matrix4X4 TranslationMatrix( const Vector2& translation );
	static Matrix4X4 TranslationMatrix( const Vector3& translation );

	//Identity
	//
	void MakeIdentity();

	//Transformations
	Vector4 TransformPoint( const Vector4& point );
	Vector3 TransformPosition( const Vector3& position );
	Vector3 TransformDirection( const Vector3& direction );
	Vector2 TransformPosition( const Vector2& position );
	Vector2 TransformDirection( const Vector2& direction );

	//Getters
	Vector3 GetIBasis() const;
	Vector3 GetJBasis() const;
	Vector3 GetKBasis() const;
	Vector3 GetTranslationBasis() const;

	//Set
	void SetIBasis( const Vector3& iBasis );
	void SetJBasis( const Vector3& jBasis );
	void SetKBasis( const Vector3& kBasis );

	//Inversion
	bool InvertMatrix();

	//LookAt
	static Matrix4X4 LookAtMatrix( const Vector3& sourcePosition, const Vector3& targetPosition );

	//operator
	void operator=( const Matrix4X4& toCopy );
	Matrix4X4 operator*( const Matrix4X4& rightHandSide );

	//only for debugging purposes
	void printMatrix() const;

	float m_values[16];
};

#endif