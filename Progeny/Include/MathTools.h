#ifndef	__PROGENY_MATH_TOOLS_H__
#define __PROGENY_MATH_TOOLS_H__

#include <string>
#include <sstream>

namespace Progeny 
{
	/*! The ratio of a circle's circumference to its diameter. */
	static const double PI = 3.14159265358979323846;
	static const double RAD_TO_DEG = 180.0f/PI;

	/*! Returns the closest integer value to the provided double. */
	int round(double value);

	/*! Returns a reference to the smaller argument. */
	
	float minf(float a, float b);

	float maxf(float a, float b);

	void clamp(int& val, int minVal, int maxVal);

	void clampf(float& val, float minVal, float maxVal);

	void floatToWstring( float val, std::wstring& ws, int precision);
}

#endif