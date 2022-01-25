#ifndef __PROGENYDEMO_EXTENDEDCAMERA_H__
#define __PROGENYDEMO_EXTENDEDCAMERA_H__

#include <Graphics/Camera.h>
#include <Math/MathTypes.h>
#include "DemoTypes.h"

class ExtendedCamera
{
public:
	ExtendedCamera();
	virtual ~ExtendedCamera();

	// instantly update the camera position (no inertia effect)
	void instantUpdate(Horizon::VECTOR3 cameraPosition, Horizon::VECTOR3 targetPosition = Horizon::VECTOR3(0.0f,0.0f,0.0f));
	
	// Updates the positions of the interanl camera positions at a pace governed by the class tightness value, producing a soft movement effect
	void update(double elapsedTime, Horizon::VECTOR3 cameraPosition, Horizon::VECTOR3 targetPosition = Horizon::VECTOR3(0.0f,0.0f,0.0f));

	// Versions of Update and instant update which accept Spherical Coordiantes
	void instantUpdateSpherical(const SPHERECOORDS& newSphereCoords, Horizon::VECTOR3 targetPosition = Horizon::VECTOR3(0.0f,0.0f,0.0f));
	void updateSpherical(double elapsedTime, const SPHERECOORDS& newSphereCoords, Horizon::VECTOR3 targetPosition = Horizon::VECTOR3(0.0f,0.0f,0.0f));

	// Used at the moment so that the graphics object can grab a reference to the camera to set it (required by horizon)
	Horizon::CCamera* getCamera();

	inline void setViewTransform(Horizon::MATRIX44 *viewTransform) {_camera.SetViewTransform(viewTransform);}
	inline Horizon::MATRIX44* getViewTransform() {return _camera.GetViewTransform();}

	inline void setTightness(double tightness) {_tightness = tightness;}
	inline double getTightness() {return _tightness;}

	// Create the view transform from the current world camera position and the camera orientation quaternion
	void constructViewTransform();

	Horizon::VECTOR3 _cameraPositionWorld;
	Horizon::QUATERNION _cameraOrientation;

protected:

private:
	Horizon::CCamera _camera;

	// Stored so that extended camera can do interpolation over an arc rather than a line
	SPHERECOORDS _cameraSphereCoords;

	float _tightness;
	static const int realizationConstant = 5;
};

#endif