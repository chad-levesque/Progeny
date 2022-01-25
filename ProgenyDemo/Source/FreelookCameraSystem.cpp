#include "FreelookCameraSystem.h"
#include "Math/Math.h"
#include <System/System.h> // used to grab deltatime
#include "DemoConstants.h"

const float FreelookCameraSystem::_displacementStep(1.0);
const float FreelookCameraSystem::_displacementStepRotation(0.04);

FreelookCameraSystem::FreelookCameraSystem(float planetRadius) :
	CameraSystem(planetRadius),
	_alphaDelta(0.0f),
	_betaDelta(0.0f),
	_gammaDelta(0.0f),
	_cameraPositionDelta(),
	_cameraPositionDeltaPrevFrame(),
	_quatCurrent(0.0f,0.0f,0.0f,1.0f), // x,y,z = 0, w = 1 represents identity quaternion (no rotation)
	_deltaTimePrevUpdate(0.0f)
{
	Horizon::VECTOR3 location(0.0f, 0.0f, -(_planetRadius * 8));
	_extCamera._cameraPositionWorld = location;
}

FreelookCameraSystem::~FreelookCameraSystem()
{

}

void FreelookCameraSystem::update()
{
	evalDeltatime();

	// Find our local axis from our current orientation
	Horizon::QUATERNION qtemp = _quatCurrent;
	qtemp.x = -_quatCurrent.x;
	qtemp.y = -_quatCurrent.y;
	qtemp.z = -_quatCurrent.z;
	Horizon::MATRIX44 mtemp;
	MatrixQuaternion(&mtemp, &qtemp);

	Horizon::VECTOR3 xAxisLocal(1.0f, 0.0f, 0.0f);
	Horizon::VECTOR3 yAxisLocal(0.0f, 1.0f, 0.0f);
	Horizon::VECTOR3 zAxisLocal(0.0f, 0.0f, 1.0f);

	VectorTransform(&xAxisLocal, &mtemp);
	VectorTransform(&yAxisLocal, &mtemp);
	VectorTransform(&zAxisLocal, &mtemp);

	// Find the new single axis rotation quaternions with delta rotation applied
	Horizon::QUATERNION xrot;
	Horizon::QUATERNION yrot;
	Horizon::QUATERNION zrot;

	QuaternionFromAxisAngle(&xrot, &xAxisLocal, _alphaDelta);
	QuaternionFromAxisAngle(&yrot, &yAxisLocal, _betaDelta);
	QuaternionFromAxisAngle(&zrot, &zAxisLocal, _gammaDelta);

	// Concatenate these rotations into the previous orientation quaternion
	Horizon::QUATERNION lastCameraDir = _quatCurrent;
	Horizon::QUATERNION temp;
	Horizon::QUATERNION temp2;
	QuaternionMultiply(&temp, &lastCameraDir, &xrot);
	QuaternionMultiply(&temp2, &temp, &yrot);
	QuaternionMultiply(&_quatCurrent, &temp2, &zrot);

	// Transform the delta translation to world space and apply it to the camera position
	VectorTransform(&_cameraPositionDelta, &mtemp);
	_extCamera._cameraPositionWorld.x += _cameraPositionDelta.x;
	_extCamera._cameraPositionWorld.y += _cameraPositionDelta.y;
	_extCamera._cameraPositionWorld.z += _cameraPositionDelta.z;

	// Set the new orientation in the camera object and tell it to construct the view matrix
	_extCamera._cameraOrientation = _quatCurrent;
	_extCamera.constructViewTransform();

	resetDeltas();
}

void FreelookCameraSystem::moveUp(float moveMultipler)
{
	_gammaDelta -= (_displacementStepRotation * _deltaTimePrevUpdate * 10) * moveMultipler;
}

void FreelookCameraSystem::moveDown(float moveMultipler)
{
	_gammaDelta += (_displacementStepRotation * _deltaTimePrevUpdate * 10) * moveMultipler;
}

void FreelookCameraSystem::moveLeft(float moveMultipler)
{
	_cameraPositionDelta.x -= (_displacementStep * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void FreelookCameraSystem::moveRight(float moveMultipler)
{
	_cameraPositionDelta.x += (_displacementStep * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void FreelookCameraSystem::moveForward(float moveMultipler)
{
	_cameraPositionDelta.z += (_displacementStep * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void FreelookCameraSystem::moveBackwards(float moveMultipler)
{
	_cameraPositionDelta.z -= (_displacementStep * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void FreelookCameraSystem::spinCamera(float xRelative, float yRelative)
{
	_betaDelta -= xRelative * _deltaTimePrevUpdate * _displacementStepRotation;
	_alphaDelta -= yRelative * _deltaTimePrevUpdate * _displacementStepRotation;
}

void FreelookCameraSystem::adjustRadius(float zRelative)
{
	_cameraPositionDelta.y += zRelative * _deltaTimePrevUpdate * _displacementStep * _cameraMovementSpeed * _distToSurface;
}

void FreelookCameraSystem::setCameraPosition(const Horizon::VECTOR3& newPos)
{
	_extCamera._cameraPositionWorld = newPos;

	//TEST: have it look at 0,0,0 after position is set (XXX: will prob leave this in as this is the behaviour we need now)
	Horizon::VECTOR3 origin(0.0f,0.0f,0.0f);
	lookAtWorldPoint(origin);
}

void FreelookCameraSystem::collide()
{
	// XXX: This routine currently takes advantage of the fact that the planet center will always be at (0,0,0), it will break if this changes
	Horizon::VECTOR3 worldPosVector(_extCamera._cameraPositionWorld);
	VectorNormalize(&worldPosVector);

	worldPosVector *= NEAR_CLIPPING_DISTANCE + 10.0f;
	_extCamera._cameraPositionWorld += worldPosVector;

	_extCamera.constructViewTransform();
}

void FreelookCameraSystem::resetCameraPosition()
{
	resetDeltas();
	_extCamera.instantUpdateSpherical(INITAL_CAMERA_POS);
}

CameraSystemTypes FreelookCameraSystem::getType()
{
	return CameraSystemTypes::FREELOOK;
}

Horizon::QUATERNION FreelookCameraSystem::getCameraOrientation()
{
	return _quatCurrent;
}

void FreelookCameraSystem::evalDeltatime()
{
	float deltaTime = Horizon::CSystem::GetInstance()->GetSystemClock()->GetDeltaTime();
	_deltaTimePrevUpdate = deltaTime;
}

void FreelookCameraSystem::resetDeltas()
{
	_cameraPositionDeltaPrevFrame = _cameraPositionDelta; // XXX: Facilitates collision resolution atm

	_cameraPositionDelta.x = 0.0f; 
	_cameraPositionDelta.y = 0.0f; 
	_cameraPositionDelta.z = 0.0f;

	_alphaDelta = 0.0f;
	_betaDelta = 0.0f;
	_gammaDelta = 0.0f;
}

void FreelookCameraSystem::lookAtWorldPoint(Horizon::VECTOR3& toLookAt)
{
	Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	_extCamera.getCamera()->SetViewLookAt(&_extCamera._cameraPositionWorld, &toLookAt, &up);

	Horizon::MATRIX44* tmpMat = _extCamera.getCamera()->GetViewTransform();
	MatrixToQuaternion(&_quatCurrent, tmpMat);
}