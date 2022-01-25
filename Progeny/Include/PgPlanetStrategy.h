#ifndef __PG_PLANET_STRATEGY__
#define __PG_PLANET_STRATEGY__

#include "PgVector3.h"

namespace Progeny
{
    /** An interface that planet strategies should implement
	 *  in order to provide PgROAMPlanet with an algorithm for
	 *  building a planet's height map.
	 */
	class PgPlanetStrategy
	{
	public:
		PgPlanetStrategy() {};
		virtual ~PgPlanetStrategy() {};
		virtual void getSurfaceData(const PgVector3& sampleDir, float& elevationOut, unsigned int& colorOut) const = 0;
		virtual float getBaseRadius() const = 0;
	};
}

#endif //__PG_PLANET_STRATEGY__