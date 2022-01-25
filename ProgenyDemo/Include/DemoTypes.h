#ifndef __PROGENYDEMO_DEMOTYPES_H__
#define __PROGENYDEMO_DEMOTYPES_H__

#include <Math/MathTypes.h>
#include <Math/MathTools.h>

struct SPHERECOORDS
{
	union
	{
		struct  
		{
			float _radius;
			float _inclination;
			float _azimuth;
		};
	};

	SPHERECOORDS();
	SPHERECOORDS(float radius, float inclination, float azimuth);
};

// HELPER FUNCTIONS BELOW

inline void SPHERICALTOCARTESIAN(Horizon::VECTOR3* out, const SPHERECOORDS* in)
{
	// Assumes Right Hand Coord System, Sphere Coords in Degrees
	out->x = in->_radius * sin(Horizon::DegreesToRadians(in->_inclination)) * cos(Horizon::DegreesToRadians(in->_azimuth));
	out->y = in->_radius * cos(Horizon::DegreesToRadians(in->_inclination));
	out->z = in->_radius * sin(Horizon::DegreesToRadians(in->_inclination)) * sin(Horizon::DegreesToRadians(in->_azimuth));
};

inline void CARTESIANTOSPHERICAL(SPHERECOORDS* out, const Horizon::VECTOR3* in)
{
	// Assumes Right Hand Coord System, Sphere Coords in Degrees
	out->_radius = sqrt(pow(in->x, 2) + pow(in->y, 2) + pow(in->z, 2));
	out->_inclination = Horizon::RadiansToDegrees(acos(in->y / out->_radius));
	out->_azimuth = Horizon::RadiansToDegrees(atan2(in->z , in->x));
}

#endif // __PROGENYDEMO_DEMOTYPES_H__