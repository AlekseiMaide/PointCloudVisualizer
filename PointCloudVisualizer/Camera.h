#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// ---------------------------------------------------
// ABSTRACT CAMERA BASE CLASS.
// ---------------------------------------------------

class Camera
{
public:
	~Camera();

	glm::mat4 getViewMatrix() const;

	virtual void setPosition(const glm::vec3& position) {}
	virtual void rotate(float yaw, float pitch) {} // In degrees.
	virtual void move(const glm::vec3& offsetPos) {}

	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;

	float getFOV() { return mFOV; }
	void setFOV(float fov) { mFOV = fov; }

protected:
	Camera();

	virtual void updateCameraVectors() {}

	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mUp;
	glm::vec3 mLook;
	glm::vec3 mRight;
	const glm::vec3 WORLD_UP;

	// Euler's angles in radians
	float mYaw;
	float mPitch;

	// Camera parameters
	float mFOV; // In degrees.
};

// ---------------------------------------------------
// FPS CAMERA CLASS.
// ---------------------------------------------------

class FPSCamera : public Camera
{
public:
	// Yaw initial angle fazes -Z axis.
	FPSCamera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		float yaw = glm::pi<float>(), 
		float pitch = 0.0f); 

	virtual void setPosition(const glm::vec3& position);
	virtual void rotate(float yaw, float pitch);
	virtual void move(const glm::vec3& offsetPos);
private:
	void updateCameraVectors();
};

// ---------------------------------------------------
// ORBIT / SPHERICAL CAMERA CLASS.
// ---------------------------------------------------

class OrbitCamera : public Camera
{
public:
	OrbitCamera();

	virtual void rotate(float yaw, float pitch); // In degrees.

	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);

private:
	void updateCameraVectors();
	float mRadius;

};

#endif
