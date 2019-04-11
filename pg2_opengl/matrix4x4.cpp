#include "pch.h"
#include "matrix4x4.h"
#include "utils.h"

Matrix4x4::Matrix4x4()
{
	for ( int r = 0; r < 4; ++r )
	{
		for ( int c = 0; c < 4; ++c )
		{
			data_[c + r * 4] = ( ( r == c ) ? 1.0f : 0.0f );
		}
	}
}

Matrix4x4::Matrix4x4( const float m00, const float m01, const float m02, const float m03, 
		const float m10, const float m11, const float m12, const float m13, 
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33 )
{
	m00_ = m00;
	m01_ = m01;
	m02_ = m02;
	m03_ = m03;

	m10_ = m10;
	m11_ = m11;
	m12_ = m12;
	m13_ = m13;

	m20_ = m20;
	m21_ = m21;
	m22_ = m22;
	m23_ = m23;

	m30_ = m30;
	m31_ = m31;
	m32_ = m32;
	m33_ = m33;
}

Matrix4x4::Matrix4x4( const Vector3 & axis_x, const Vector3 & axis_y, const Vector3 & axis_z,
	const Vector3 & view_from )
{
	m00_ = axis_x.x;
	m01_ = axis_y.x;
	m02_ = axis_z.x;
	m03_ = view_from.x;

	m10_ = axis_x.y;
	m11_ = axis_y.y;
	m12_ = axis_z.y;
	m13_ = view_from.y;

	m20_ = axis_x.z;
	m21_ = axis_y.z;
	m22_ = axis_z.z;
	m23_ = view_from.z;

	m30_ = 0.0f;
	m31_ = 0.0f;
	m32_ = 0.0f;
	m33_ = 1.0f;
}

void Matrix4x4::Transpose()
{	
	float tmp = m01_;
	m01_ = m10_;
	m10_ = tmp;

	tmp = m02_;
	m02_ = m20_;
	m20_ = tmp;

	tmp = m03_;
	m03_ = m30_;
	m30_ = tmp;

	tmp = m12_;
	m12_ = m21_;
	m21_ = tmp;	

	tmp = m13_;
	m13_ = m31_;
	m31_ = tmp;

	tmp = m23_;
	m23_ = m32_;
	m32_ = tmp;
}

void Matrix4x4::EuclideanInverse()
{
	const float m03 = -m00_ * m03_ - m10_ * m13_ - m20_ * m23_;
	const float m13 = -m01_ * m03_ - m11_ * m13_ - m21_ * m23_;
	const float m23 = -m02_ * m03_ - m12_ * m13_ - m22_ * m23_;

	m03_ = m03;
	m13_ = m13;
	m23_ = m23;

	float tmp = m01_;
	m01_ = m10_;
	m10_ = tmp;

	tmp = m02_;
	m02_ = m20_;
	m20_ = tmp;

	tmp = m12_;
	m12_ = m21_;
	m21_ = tmp;	
}

Matrix4x4 Matrix4x4::EuclideanInverse( Matrix4x4 m )
{
	Matrix4x4 m_inv;

	memcpy( m_inv.data(), m.data(), sizeof( m_inv.data_ ) );
	
	m_inv.m03_ = -m.m00_ * m.m03_ - m.m10_ * m.m13_ - m.m20_ * m.m23_;
	m_inv.m13_ = -m.m01_ * m.m03_ - m.m11_ * m.m13_ - m.m21_ * m.m23_;
	m_inv.m23_ = -m.m02_ * m.m03_ - m.m12_ * m.m13_ - m.m22_ * m.m23_;	

	swap<float>( m_inv.m01_, m_inv.m10_ );
	swap<float>( m_inv.m02_, m_inv.m20_ );
	swap<float>( m_inv.m12_, m_inv.m21_ );	

	return m_inv;
}

void Matrix4x4::set( const int row, const int column, const float value )
{
	assert( row >= 0 && row < 4 && column >= 0 && column < 4 );

	data_[column + row * 4] = value;
}

float Matrix4x4::get( const int row, const int column ) const
{
	assert( row >= 0 && row < 4 && column >= 0 && column < 4 );

	return data_[column + row * 4];
}

float * Matrix4x4::data()
{
	return &data_[0];
}

Matrix3x3 Matrix4x4::so3() const
{
	return Matrix3x3( m00_, m01_, m02_,
		m10_, m11_, m12_,
		m20_, m21_, m22_ );
}

Vector3 Matrix4x4::tr3() const
{
	return Vector3( m03_, m13_, m23_ );
}

void Matrix4x4::so3( const Matrix3x3 & m )
{
	for ( int i = 0; i < 3; ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			set( i, j, m.get( i, j ) );
		}
	}	
}

void Matrix4x4::tr3(const Vector3 & v)
{
	m03_ = v.x;
	m13_ = v.y;
	m23_ = v.z;
}

std::string Matrix4x4::toString() const
{		
	return std::to_string( m00_ ) + " " + std::to_string( m01_ ) + " " + std::to_string( m02_ ) + " " + std::to_string( m03_ ) + "\n" +
		std::to_string( m10_ ) + " " + std::to_string( m11_ ) + " " + std::to_string( m12_ ) + " " + std::to_string( m13_ ) + "\n" +
		std::to_string( m20_ ) + " " + std::to_string( m21_ ) + " " + std::to_string( m22_ ) + " " + std::to_string( m23_ ) + "\n" +
		std::to_string( m30_ ) + " " + std::to_string( m31_ ) + " " + std::to_string( m32_ ) + " " + std::to_string( m33_ ) + "\n";
}

bool Matrix4x4::operator==( const Matrix4x4 & m ) const
{
	for ( int j = 0; j < 4; ++j )
	{
		for ( int i = 0; i < 4; ++i )
		{
			if ( get( i, j ) != m.get( i, j ) ) return false; // FIX this
		}
	}

	return true;
}

/*Vector4 operator*( const Matrix4x4 & a, const Vector4 & b )
{
	return Vector4( a.m00_ * b.x + a.m01_ * b.y + a.m02_ * b.z + a.m03_ * b.w,
		a.m10_ * b.x + a.m11_ * b.y + a.m12_ * b.z + a.m13_ * b.w,
		a.m20_ * b.x + a.m21_ * b.y + a.m22_ * b.z + a.m23_ * b.w,
		a.m30_ * b.x + a.m31_ * b.y + a.m32_ * b.z + a.m33_ * b.w );
}*/

Matrix4x4 operator*( const Matrix4x4 & a, const Matrix4x4 & b )
{
	return Matrix4x4( a.m00_ * b.m00_ + a.m01_ * b.m10_ + a.m02_ * b.m20_ + a.m03_ * b.m30_,
		a.m00_ * b.m01_ + a.m01_ * b.m11_ + a.m02_ * b.m21_ + a.m03_ * b.m31_,
		a.m00_ * b.m02_ + a.m01_ * b.m12_ + a.m02_ * b.m22_ + a.m03_ * b.m32_,
		a.m00_ * b.m03_ + a.m01_ * b.m13_ + a.m02_ * b.m23_ + a.m03_ * b.m33_,

		a.m10_ * b.m00_ + a.m11_ * b.m10_ + a.m12_ * b.m20_ + a.m13_ * b.m30_,
		a.m10_ * b.m01_ + a.m11_ * b.m11_ + a.m12_ * b.m21_ + a.m13_ * b.m31_,
		a.m10_ * b.m02_ + a.m11_ * b.m12_ + a.m12_ * b.m22_ + a.m13_ * b.m32_,
		a.m10_ * b.m03_ + a.m11_ * b.m13_ + a.m12_ * b.m23_ + a.m13_ * b.m33_,

		a.m20_ * b.m00_ + a.m21_ * b.m10_ + a.m22_ * b.m20_ + a.m23_ * b.m30_,
		a.m20_ * b.m01_ + a.m21_ * b.m11_ + a.m22_ * b.m21_ + a.m23_ * b.m31_,
		a.m20_ * b.m02_ + a.m21_ * b.m12_ + a.m22_ * b.m22_ + a.m23_ * b.m32_,
		a.m20_ * b.m03_ + a.m21_ * b.m13_ + a.m22_ * b.m23_ + a.m23_ * b.m33_,

		a.m30_ * b.m00_ + a.m31_ * b.m10_ + a.m32_ * b.m20_ + a.m33_ * b.m30_,
		a.m30_ * b.m01_ + a.m31_ * b.m11_ + a.m32_ * b.m21_ + a.m33_ * b.m31_,
		a.m30_ * b.m02_ + a.m31_ * b.m12_ + a.m32_ * b.m22_ + a.m33_ * b.m32_,
		a.m30_ * b.m03_ + a.m31_ * b.m13_ + a.m32_ * b.m23_ + a.m33_ * b.m33_ );
}
