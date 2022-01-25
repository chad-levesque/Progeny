#ifndef __PROGENYDEMO_FREELOOKCAMERASYSTEM_H__
#define __PROGENYDEMO_FREELOOKCAMERASYSTEM_H__

#include "CameraSystem.h"
#include <Math/MathTypes.h>

class FreelookCameraSystem : public CameraSystem
{
public:
	FreelookCameraSystem(float planetRadius);
	virtual ~FreelookCameraSystem();

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

	Horizon::QUATERNION getCameraOrientation();
protected:



private:
	void evalDeltatime();
	void resetDeltas();
	void lookAtWorldPoint(Horizon::VECTOR3& toLookAt);

	float _deltaTimePrevUpdate;
	Horizon::VECTOR3 _cameraPositionDelta;
	Horizon::VECTOR3 _cameraPositionDeltaPrevFrame; // TODO: get rid of this

	float _alphaDelta;
	float _betaDelta;
	float _gammaDelta;

	Horizon::QUATERNION _quatCurrent;

	static const float _displacementStep;
	static const float _displacementStepRotation;
};


#endif // __PROGENYDEMO_FREELOOKCAMERASYSTEM_H__