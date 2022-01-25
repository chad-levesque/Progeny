#ifndef __PG_PLANET_STRATEGY_FLAT__
#define __PG_PLANET_STRATEGY_FLAT__

#include "PgPlanetStrategy.h"

namespace Progeny
{
	/** A concrete implementation of PgPlanetStrategy that builds
	 *  perfectly spherical planets
	 */
	class PgPlanetStrategyFlat : public PgPlanetStrategy
	{
	public:
		PgPlanetStrategyFlat(float radius);
		~PgPlanetStrategyFlat() {};
		virtual float GetElevation(const PgVector3& v) const;
		virtual float getBaseRadius() const;
		virtual unsigned int getPlanetColor( float elevationOffset ) const;

	private:
		float _baseRadius;
	};
}

#endif //__PG_PLANET_STRATEGY_FLAT__