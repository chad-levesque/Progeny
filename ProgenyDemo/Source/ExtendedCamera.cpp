#include "ExtendedCamera.h"

#include "DemoConstants.h"

#include <Math/Math.h>
#include <Math/MathTypes.h>
#include <Math/MathTools.h>
#include <Graphics/Graphics.h> // TODO: try to avoid needing this

ExtendedCamera::ExtendedCamera() : 
	_cameraSphereCoords(),
	_cameraPositionWorld(),
	_cameraOrientation(0.0f,0.0f,0.0f,1.0f),
	_camera(),
	_tightness(0.9f)
{
	Horizon::IGraphics* graphics = Horizon::IGraphics::GetInstance();

	// Setup view port
	//Horizon::VECTOR3 location(200.0f, 18.0f, -20.0f);
	//Horizon::VECTOR3 target(0.0f, 0.0f, 0.0f);
	//Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	RECTI vp;
	vp.x1 = vp.y1 = 0;
	vp.x2 = graphics->GetDisplayWidth();
	vp.y2 = graphics->GetDisplayHeight();
	_camera.SetViewport(&vp);
	// _camera.SetViewLookAt(&location, &target, &up);
	_camera.SetProjPerspective(float(PI) / 4.0f, NEAR_CLIPPING_DISTANCE, FAR_CLIPPING_DISTANCE);
}

ExtendedCamera::~ExtendedCamera()
{
}

void ExtendedCamera::instantUpdate(Horizon::VECTOR3 cameraPosition, Horizon::VECTOR3 targetPosition)
{
	_cameraPositionWorld = cameraPosition;

	Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	_camera.SetViewLookAt(&_cameraPositionWorld, &targetPosition, &up);
}

void ExtendedCamera::update(double elapsedTime, Horizon::VECTOR3 cameraPosition, Horizon::VECTOR3 targetPosition)
{
	Horizon::VECTOR3 displacement(cameraPosition.x - _cameraPositionWorld.x, cameraPosition.y - _cameraPositionWorld.y, cameraPosition.z - _cameraPositionWorld.z);
	float scaleFactor = _tightness * (elapsedTime) * realizationConstant;

	displacement.x *= scaleFactor;
	displacement.y *= scaleFactor;
	displacement.z *= scaleFactor;

	_cameraPositionWorld.x += displacement.x;
	_cameraPositionWorld.y += displacement.y;
	_cameraPositionWorld.z += displacement.z;

	Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	_camera.SetViewLookAt(&_cameraPositionWorld, &targetPosition, &up);
}

void ExtendedCamera::instantUpdateSpherical(const SPHERECOORDS& newSphereCoords, Horizon::VECTOR3 targetPosition)
{
	_cameraSphereCoords = newSphereCoords;
	SPHERICALTOCARTESIAN(&_cameraPositionWorld, &_cameraSphereCoords);

	Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	_camera.SetViewLookAt(&_cameraPositionWorld, &targetPosition, &up);
}

void ExtendedCamera::updateSpherical(double elapsedTime, const SPHERECOORDS& newSphereCoords, Horizon::VECTOR3 targetPosition)
{
	float scaleFactor = _tightness * (elapsedTime) * realizationConstant;

	_cameraSphereCoords._radius += (newSphereCoords._radius - _cameraSphereCoords._radius) * scaleFactor;
	_cameraSphereCoords._inclination += (newSphereCoords._inclination - _cameraSphereCoords._inclination) * scaleFactor;
	_cameraSphereCoords._azimuth += (newSphereCoords._azimuth - _cameraSphereCoords._azimuth) * scaleFactor;

	SPHERICALTOCARTESIAN(&_cameraPositionWorld, &_cameraSphereCoords);

	Horizon::VECTOR3 up(0.0f, 1.0f, 0.0f);
	_camera.SetViewLookAt(&_cameraPositionWorld, &targetPosition, &up);
}

Horizon::CCamera* ExtendedCamera::getCamera()
{
	return &_camera;
}

void ExtendedCamera::constructViewTransform()
{
	Horizon::MATRIX44* viewTransform  = _camera.GetViewTransform();

	// Create the view transform matrix from the orientation quaternion and the world space camera position
	MatrixQuaternion(viewTransform, &_cameraOrientation);

	Horizon::VECTOR3 newXAxis(viewTransform->m11, viewTransform->m12, viewTransform->m13);
	Horizon::VECTOR3 newYAxis(viewTransform->m21, viewTransform->m22, viewTransform->m23);
	Horizon::VECTOR3 newZAxis(viewTransform->m31, viewTransform->m32, viewTransform->m33);
	Horizon::VECTOR3 location(_cameraPositionWorld.x, _cameraPositionWorld.y, _cameraPositionWorld.z);

	viewTransform->m14 = -(VectorDotProduct(&newXAxis, &location));
	viewTransform->m24 = -(VectorDotProduct(&newYAxis, &location));
	viewTransform->m34 = -(VectorDotProduct(&newZAxis, &location));

	setViewTransform(viewTransform);
}