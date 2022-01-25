#ifndef __PROGENYDEMO_ORBITALCAMERASYSTEM_H__
#define __PROGENYDEMO_ORBITALCAMERASYSTEM_H__

#include "CameraSystem.h"
#include "DemoTypes.h"

class OrbitalCameraSystem : public CameraSystem
{
public:
	OrbitalCameraSystem(float planetRadius);
	virtual ~OrbitalCameraSystem();

	virtual void update();
	void moveUp(float moveMultiplier = 1.0f);
	void moveDown(float moveMultiplier = 1.0f); 
	void moveLeft(float moveMultiplier = 1.0f);
	void moveRight(float moveMultiplier = 1.0f);
	void moveForward(float moveMultiplier = 1.0f);
	void moveBackwards(float moveMultiplier = 1.0f);
	void spinCamera(float xRelative, float yRelative);
	void adjustRadius(float zRelative);
	void setCameraPosition(const Horizon::VECTOR3& newPos);
	void collide();
	void resetCameraPosition();
	CameraSystemTypes getType();

	// When changing to orbital camera a previous orientation can be set to allow for a smooth transition of orientation
	void setSlerpFrom(Horizon::QUATERNION quatSlerpFrom);
protected:

private:
	void normalizeSphericalValues();

	SPHERECOORDS _sphereCoords;

	double _deltaTimePrevUpdate;
	float _radiusDeltaPrevFrame;

	static const float _displacementStepRadius;
	static const float _displacementStepDegrees;

	// For slerping from an orientation when switching to orbital
	Horizon::QUATERNION _quatSlerpTo;
	Horizon::QUATERNION _quatSlerpFrom;
	Horizon::QUATERNION _quatSlerpIntermediate;
	SPHERECOORDS _sphereCoordsBeforeSlerp;
	double _slerpTimeMilisec;
	bool _bPerformingSlerp;
	void performSlerp(double deltaTime);
};


#endif // __PROGENYDEMO_ORBITALCAMERASYSTEM_H__