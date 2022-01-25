#ifndef __PG_PLANET_STRATEGY_RANDOM__
#define __PG_PLANET_STRATEGY_RANDOM__

#include "PgPlanetStrategy.h"
#include "PgPlanetAttributes.h"
#include "PgColorScheme.h"
#include "PgPlanetModule.h"
#include "libnoise/noise.h"
#include <vector>

namespace Progeny
{
	class PgColorScheme;

	/** A concrete implementation of PgPlanetStrategy that builds
	 *  planets using 3D Perlin noise.
	 */
	class PgPlanetStrategyRandom : public PgPlanetStrategy
	{
	public:
		PgPlanetStrategyRandom( const PgPlanetAttributes& attributes, const PgColorScheme& colorScheme );
		virtual ~PgPlanetStrategyRandom() {};
		virtual void getSurfaceData(const PgVector3& sampleDir, float& elevationOut, unsigned int& colorOut) const;
		virtual float getBaseRadius() const { return _attributes._radius; }
	private:

		PgPlanetModule _heightMap;
		const PgColorScheme _colorScheme;
		const PgPlanetAttributes _attributes;

		float _sampleRadius;
	};
}
#endif //__PG_PLANET_STRATEGY_RANDOM__