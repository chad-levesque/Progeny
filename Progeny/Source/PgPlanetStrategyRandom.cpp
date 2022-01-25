#include "PgPlanetStrategyRandom.h"

#include "MathTools.h"
#include "PgConstants.h"

using namespace Progeny;

PgPlanetStrategyRandom::PgPlanetStrategyRandom( const PgPlanetAttributes& attributes, const PgColorScheme& colorScheme )
	: _heightMap(attributes),
	  _colorScheme(colorScheme),
	  _attributes(attributes),
	  _sampleRadius(attributes._radius/1000.0f)
{
	_heightMap.SetSeed(attributes._seed);
	_heightMap.SetCraterCount(1.0f);
	_heightMap.SetCraterIntensity(0.75f);
	_heightMap.SetCraterScale(2.0f);
}

void
PgPlanetStrategyRandom::getSurfaceData(const PgVector3& sampleDir, float& elevationOut, unsigned int& colorOut) const
{
	// Calculate vertex elevation:
	PgVector3 samplePoint = sampleDir;
	samplePoint.normalize();
	float yMag = abs(samplePoint._y);
	samplePoint*= _sampleRadius;
	float noiseValue = (float)_heightMap.GetValue(samplePoint._x, samplePoint._y, samplePoint._z);
	noiseValue *= _attributes._noiseAmplitude*_attributes._radius;
	float elevation = noiseValue + _attributes._radius;
	elevationOut = elevation;

	// Calculate vertex color:
	
	elevation -=_attributes._radius;
	elevation /= _attributes._noiseAmplitude*_attributes._radius;
	elevation = 0.5f*(elevation + 1.0f);
	clampf(elevation, 0.0f, 1.0f);
	unsigned int color = 0xFFFFFFFF;

	int ranges = _colorScheme.nNumRanges;
	float polarLat = _colorScheme.fPolarLatitude;
	float blendFactor = 0.0f;
	if (yMag > polarLat)
	{
		blendFactor = (yMag - polarLat)/(1.0f - polarLat);
	}

	for (int i = 0; i < ranges; ++i)
	{
		if ( elevation <= _colorScheme.fRangeHeights[i])
		{
			if (i < ranges-1 && blendFactor > 0.0f)
			{
				color = PgColorScheme::blendColor(blendFactor, _colorScheme.iRangeColors[i], _colorScheme.iRangeColors[ranges-1]);
			}
			else
			{
				color = _colorScheme.iRangeColors[i];
			}
			break;
		}
		
	}
	colorOut = color;
}