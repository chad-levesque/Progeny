#include "OrbitalCameraSystem.h"

#include "DemoConstants.h"
#include <Math/MathTools.h>
#include <Math.h>
#include <System/System.h>

const float OrbitalCameraSystem::_displacementStepRadius(0.3f);
const float OrbitalCameraSystem::_displacementStepDegrees(100.0f);

#define slerpTime 1.0

OrbitalCameraSystem::OrbitalCameraSystem(float planetRadius) :
	CameraSystem(planetRadius),
	_sphereCoords(INITAL_CAMERA_POS),
	_deltaTimePrevUpdate(0.0),
	_quatSlerpFrom(),
	_quatSlerpTo(),
	_quatSlerpIntermediate(),
	_sphereCoordsBeforeSlerp(),
	_slerpTimeMilisec(slerpTime),
	_bPerformingSlerp(false)
{
	// Initialize the extended camera spherical values
	_extCamera.instantUpdateSpherical(_sphereCoords);
}

OrbitalCameraSystem::~OrbitalCameraSystem()
{

}

void OrbitalCameraSystem::update()
{
	double deltaTime = Horizon::CSystem::GetInstance()->GetSystemClock()->GetDeltaTime();
	_deltaTimePrevUpdate = deltaTime;

	if(_bPerformingSlerp == false)
	{
		normalizeSphericalValues();

		if(_deltaTimePrevUpdate > 0.1)
		{
			_extCamera.instantUpdateSpherical(_sphereCoords);
		}

		_extCamera.updateSpherical(_deltaTimePrevUpdate, _sphereCoords);
	}
	else // _bPerformingSlerp == true
	{
		performSlerp(deltaTime);
	}
}

void OrbitalCameraSystem::moveUp(float moveMultipler)
{
	_sphereCoords._inclination += (_displacementStepDegrees * _deltaTimePrevUpdate * _cameraMovementSpeed) * 0.0008 * ((1/log(_distToSurface)) * min(_distToSurface, 8000)) * moveMultipler;
}

void OrbitalCameraSystem::moveDown(float moveMultipler)
{
	_sphereCoords._inclination -= (_displacementStepDegrees * _deltaTimePrevUpdate * _cameraMovementSpeed) * 0.0008 * ((1/log(_distToSurface)) * min(_distToSurface, 8000)) * moveMultipler;
}

void OrbitalCameraSystem::moveLeft(float moveMultipler)
{
	_sphereCoords._azimuth -= (_displacementStepDegrees * _deltaTimePrevUpdate * _cameraMovementSpeed) * 0.0016 * ((1/log(_distToSurface)) * min(_distToSurface, 8000)) * moveMultipler;
}

void OrbitalCameraSystem::moveRight(float moveMultipler)
{
	_sphereCoords._azimuth += (_displacementStepDegrees * _deltaTimePrevUpdate * _cameraMovementSpeed) * 0.0016 * ((1/log(_distToSurface)) * min(_distToSurface, 8000)) * moveMultipler;
}

void OrbitalCameraSystem::moveForward(float moveMultipler)
{
	_sphereCoords._radius -= (_displacementStepRadius * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void OrbitalCameraSystem::moveBackwards(float moveMultipler)
{
	_sphereCoords._radius += (_displacementStepRadius * _deltaTimePrevUpdate * _cameraMovementSpeed * _distToSurface) * moveMultipler;
}

void OrbitalCameraSystem::spinCamera(float xRelative, float yRelative)
{
	 _sphereCoords._inclination -= yRelative * _deltaTimePrevUpdate * 0.0016 * ((1/log(_distToSurface)) * min(_distToSurface, 8000));
	 _sphereCoords._azimuth -= xRelative * _deltaTimePrevUpdate * 0.0032  * ((1/log(_distToSurface)) * min(_distToSurface, 8000)); // TODO: Remove magic number
}

void OrbitalCameraSystem::adjustRadius(float zRelative)
{
	_sphereCoords._radius -= zRelative * (_displacementStepRadius * 0.1 * _distToSurface);
}

void OrbitalCameraSystem::collide()
{
	_sphereCoords._radius += 1000;
	_extCamera.updateSpherical(_deltaTimePrevUpdate, _sphereCoords);
}

void OrbitalCameraSystem::resetCameraPosition()
{
	_sphereCoords = SPHERECOORDS(INITAL_CAMERA_POS);
	_extCamera.instantUpdateSpherical(_sphereCoords);
}

CameraSystemTypes OrbitalCameraSystem::getType()
{
	return CameraSystemTypes::ORBITAL;
}

void OrbitalCameraSystem::setSlerpFrom(Horizon::QUATERNION quatSlerpFrom)
{
	_bPerformingSlerp = true;
	_slerpTimeMilisec = slerpTime;
	_quatSlerpFrom = quatSlerpFrom;
	_sphereCoordsBeforeSlerp = _sphereCoords;

	// Constructor has already casued setviewlookat for target 0,0,0 to occur
	MatrixToQuaternion(&_quatSlerpTo, _extCamera.getViewTransform());

}

void OrbitalCameraSystem::performSlerp(double deltaTime)
{
	_slerpTimeMilisec -= deltaTime;
	if(_slerpTimeMilisec > 0.0)
	{
		QuaternionSlerp(&_quatSlerpIntermediate, &_quatSlerpTo, &_quatSlerpFrom, (_slerpTimeMilisec/slerpTime));

		_extCamera._cameraOrientation = _quatSlerpIntermediate;
		_extCamera.constructViewTransform();
	}
	else // slerp time elapsed
	{
		_bPerformingSlerp = false;
		_slerpTimeMilisec = slerpTime;
		_sphereCoords = _sphereCoordsBeforeSlerp; // used to ensure input during slerp dosen't cause effect
	}

}

void OrbitalCameraSystem::setCameraPosition(const Horizon::VECTOR3& newPos)
{
	CARTESIANTOSPHERICAL(&_sphereCoords, &newPos);
	_extCamera.instantUpdateSpherical(_sphereCoords);
}

void OrbitalCameraSystem::normalizeSphericalValues()
{
	if(_sphereCoords._radius < 2.0f)
	{
		_sphereCoords._radius = 2.0f;
	}

	if(_sphereCoords._inclination > 178.0f)
	{
		_sphereCoords._inclination = 178.0f;
	}
	if (_sphereCoords._inclination < 2.0f)
	{
		_sphereCoords._inclination = 2.0f;
	}

	//if (_sphereCoords._azimuth >= 360.0f)
	//{
	//	do 
	//	{
	//		_sphereCoords._azimuth -= 360.0f;
	//	} while (_sphereCoords._azimuth >= 360.0f);
	//}
	//else if (_sphereCoords._azimuth < 0.0f)
	//{
	//	do 
	//	{
	//		_sphereCoords._azimuth += 360.0f;
	//	} while (_sphereCoords._azimuth < 0.0f);

	//}
}
