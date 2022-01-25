#include "PgPlanetStrategyFlat.h"

using namespace Progeny;

PgPlanetStrategyFlat::PgPlanetStrategyFlat(float radius)
	: _baseRadius(radius)
{
}

float PgPlanetStrategyFlat::GetElevation(const PgVector3& v) const
{
	return _baseRadius;
}

float PgPlanetStrategyFlat::getBaseRadius() const
{
	return _baseRadius;
}

unsigned int PgPlanetStrategyFlat::getPlanetColor( float elevationOffset ) const
{
	unsigned int color = 0xFF000000;
	if(elevationOffset <= 0){ color = 0xFF0000FF; }
	else if(elevationOffset > 0 && elevationOffset <= 0.5){color = 0xFF00FF00; }
	else if(elevationOffset > 0.5) {color = 0xFFFF0000; }
	return color;
}
