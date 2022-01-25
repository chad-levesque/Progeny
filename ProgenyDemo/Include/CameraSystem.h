#ifndef __PROGENYDEMO_CAMERASYSTEM_H__
#define __PROGENYDEMO_CAMERASYSTEM_H__

#include "ExtendedCamera.h"
#include "DemoConstants.h"
#include <Graphics/Camera.h> // TODO: This class shouldn't need to know Camera
#include <IPgCamera.h>
#include <PgVector3.h>
#include <Math/MathTypes.h>

enum CameraSystemTypes
{
	FREELOOK,
	ORBITAL
};

class CameraSystem : public Progeny::IPgCamera
{
public:
	CameraSystem(float planetRadius);
	virtual ~CameraSystem();

	// Camera Behaviour Interface Decleration
	virtual void update() = 0;
	virtual void moveUp(float moveMultiplier = 1.0f) = 0;
	virtual void moveDown(float moveMultiplier = 1.0f) = 0;
	virtual void moveLeft(float moveMultiplier = 1.0f) = 0;
	virtual void moveRight(float moveMultiplier = 1.0f) = 0;
	virtual void moveForward(float moveMultiplier = 1.0f) = 0;
	virtual void moveBackwards(float moveMultiplier = 1.0f) = 0;
	virtual void spinCamera(float xRelative, float yRelative) = 0;
	virtual void adjustRadius(float zRelative) = 0;
	virtual void setCameraPosition(const Horizon::VECTOR3& newPos) = 0;
	virtual void collide() = 0;
	virtual void resetCameraPosition() = 0;
	virtual CameraSystemTypes getType() = 0;
	
	// IPgCamera Interface Compliance
	virtual Progeny::PgVector3 getCameraPosition();
	virtual Progeny::PgVector3 getCameraHeading();

	virtual Horizon::CCamera* getCamera();
	virtual float getCameraMovementMultiplier();
	virtual void setCameraMovementMultiplier(float movMultiplier);
	virtual void setDistanceToPlanetSurface(float distToSurface);
	

protected:
	static float _cameraMovementSpeed;

	float _planetRadius; // XXX: The camera system should not need to be aware of a planet
	float _distToSurface; // XXX: The camera system should not need to be aware of the distance to the planet surface

	ExtendedCamera _extCamera;
	
private:
	
	
};

#endif