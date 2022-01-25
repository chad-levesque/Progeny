#include "MathTools.h"

int Progeny::round(double value) 
{ 
	return static_cast<int>(value + 0.5); 
}

float Progeny::minf(float a, float b) 
{ 
	return (a<b)?a:b; 
}


float Progeny::maxf(float a, float b)
{
	return (a>b)?a:b;
}

void Progeny::clamp(int& val, int minVal, int maxVal)
{
	if(val > maxVal)
		val = maxVal;
	else if(val < minVal)
		val = minVal;
}

void Progeny::clampf(float& val, float minVal, float maxVal)
{ 
	if(val > maxVal)
		val = maxVal;
	else if(val < minVal)
		val = minVal;
}

void Progeny::floatToWstring( float val, std::wstring& ws, int precision)
{
	std::wstringstream stringconverter;
	stringconverter.precision(precision);
	stringconverter.fill('0');

	stringconverter << val;
	stringconverter >> ws;
}