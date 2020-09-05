#include "Camera.h"

#include "glm/gtx/transform.hpp"

// Camera defualt values:
const float DEF_FOV = 45.0f;

// ---------------------------------------------------
// CAMERA BASE CLASS.
// ---------------------------------------------------

Camera::Camera() 
	: mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(0.0f, 1.0f, 0.0f),
	mRight(0.0f, 0.0f, 0.0f),
	WORLD_UP(0.0f, 1.0f, 0.0f),
	mYaw(glm::pi<float>()),
	mPitch(0.0f),
	mFOV(DEF_FOV)
{

}

Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(
		mPosition,		// the position of camera, in world space
		mTargetPos,		// where you want to look at, in world space
		mUp				// glm::vec3(0,1,0), but (0,-1,0) is upside-down
	);
}

const glm::vec3& Camera::getLook() const
{
	return mLook;
}

const glm::vec3& Camera::getRight() const
{
	return mRight;
}

const glm::vec3& Camera::getUp() const
{
	return mUp;
}

// ---------------------------------------------------
// ORBIT CAMERA IMPLEMENTATION.
// ---------------------------------------------------

OrbitCamera::OrbitCamera() 
	: mRadius(10.0f)
{}

void OrbitCamera::setLookAt(const glm::vec3& target)
{
	mTargetPos = target;
}

void OrbitCamera::setRadius(float radius)
{
	mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

void OrbitCamera::rotate(float yaw, float pitch)
{
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	updateCameraVectors();
}

void OrbitCamera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system
	// NB: Y is up instead of Z, as described in wiki.
	mPosition.x = mTargetPos.x + mRadius * cosf(mPitch) * sinf(mYaw);
	mPosition.y = mTargetPos.y + mRadius * sinf(mPitch);
	mPosition.z = mTargetPos.z + mRadius * cosf(mPitch) * cosf(mYaw);
}

// ---------------------------------------------------
// FPS CAMERA IMPLEMENTATION.
// ---------------------------------------------------

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
	mPosition = position;
	mYaw = yaw;
	mPitch = pitch;
}

void FPSCamera::setPosition(const glm::vec3& position)
{
	mPosition = position;
}

// Rotate the camera based on yaw and pitch angles passed in radians.
void FPSCamera::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	// Constrain the pitch - (Pitch is steepness of a line/wave)
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	updateCameraVectors();
}

void FPSCamera::move(const glm::vec3& offsetPos)
{
	mPosition += offsetPos;
	updateCameraVectors();
}

// Calculate Vectors from the cameras's (updated) Euler angles.
void FPSCamera::updateCameraVectors()
{
	// Calculate view direction vector based on yaw and pitch angles (roll not considered)
	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	// Re-Calculate right and up vector. 
	// For simplicity the Right vector will be assumed horizontal w.r.t the World's up vector.
	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mTargetPos = mPosition + mLook;
}
