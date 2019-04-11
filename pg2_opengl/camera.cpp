#include "pch.h"
#include "camera.h"

Camera::Camera( const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at )
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	// TODO compute focal lenght based on the vertical field of view and the camera resolution
	
	// TODO build M_c_w_ matrix

	Update();
}

Vector3 Camera::view_from() const
{
	return view_from_;
}

Matrix3x3 Camera::M_c_w() const
{
	return M_c_w_;
}

float Camera::focal_length() const
{
	return f_y_;
}

void Camera::set_fov_y( const float fov_y )
{
	assert( fov_y > 0.0 );

	fov_y_ = fov_y;
}

void Camera::Update()
{
	f_y_ = height_ / ( 2.0f * tanf( fov_y_ * 0.5f ) );

	Vector3 z_c = view_from_ - view_at_;
	z_c.Normalize();
	Vector3 x_c = up_.CrossProduct( z_c );
	x_c.Normalize();
	Vector3 y_c = z_c.CrossProduct( x_c );
	y_c.Normalize();
	
	float aspectRatio = width_ / float(height_);
	float heightHalf = nearProjection * tanf(fov_y_ * 0.5);
	float widthHalf = heightHalf * aspectRatio;
	viewMatrix = Matrix4x4(x_c, y_c, z_c, view_from_);
	viewMatrix.EuclideanInverse();

	float f = 1.0f;
	float farNearFraction = (farProjection + nearProjection) / (nearProjection - farProjection);

	projectionMatrix.set(0, 0, nearProjection / widthHalf);
	projectionMatrix.set(1, 1, f * nearProjection / heightHalf);
	projectionMatrix.set(2, 2, farNearFraction);
	projectionMatrix.set(2, 3, 2 * farNearFraction);
	projectionMatrix.set(3, 2, -1);

	M_c_w_ = Matrix3x3( x_c, y_c, z_c );
}

void Camera::MoveForward( const float dt )
{
	Vector3 ds = view_at_ - view_from_;
	ds.Normalize();
	ds *= dt;

	view_from_ += ds;
	view_at_ += ds;
}
