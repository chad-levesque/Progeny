#include "CameraSystem.h"
#include <Math/Math.h>

float CameraSystem::_cameraMovementSpeed(1.0f);

CameraSystem::CameraSystem(float planetRadius) :
	_extCamera(),
	_distToSurface(1000.0f)
{
	_planetRadius = planetRadius;
}

CameraSystem::~CameraSystem()
{

}

Progeny::PgVector3 CameraSystem::getCameraPosition( ) {
	return Progeny::PgVector3(
		_extCamera._cameraPositionWorld.x,
		_extCamera._cameraPositionWorld.y,
		_extCamera._cameraPositionWorld.z );
}

Progeny::PgVector3 CameraSystem::getCameraHeading()
{
	const Horizon::MATRIX44* viewProj = getCamera()->GetViewTransform();
	Progeny::PgVector3 heading(viewProj->m31, viewProj->m32, viewProj->m33);
	heading.normalize();
	return heading;
}
Horizon::CCamera* CameraSystem::getCamera()
{
	return _extCamera.getCamera();
}

float CameraSystem::getCameraMovementMultiplier()
{
	return _cameraMovementSpeed;
}

void CameraSystem::setCameraMovementMultiplier(float movMultiplier)
{
	if(movMultiplier <= 0.25f) {movMultiplier = 0.25f; }
	if(movMultiplier >= 25.0f) { movMultiplier = 25.0f; }

	_cameraMovementSpeed = movMultiplier;
}

void CameraSystem::setDistanceToPlanetSurface(float distToSurface)
{
	if(distToSurface > 200.0)
	{
		_distToSurface = distToSurface;
	}
}