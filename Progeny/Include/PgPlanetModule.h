#ifndef __PG_PLANET_MODULE__
#define __PG_PLANET_MODULE__

#include "libnoise/../../src/module/modulebase.h"
#include "libnoise/noise.h"

#include "PgTerrainModule.h"
#include "PgCraterModule.h"

namespace Progeny
{
	struct PgPlanetAttributes;

	/** Progeny implementation of a planet height map module.
	 * This class inherits from a class internal to libnoise
	 * Should probably reside within a libnoise extension, but
	 * should be fine here for now.
	 */
	class PgPlanetModule : public noise::module::Module
	{
	public:
		PgPlanetModule(const PgPlanetAttributes& attribues);
		virtual double GetValue (double x, double y, double z) const;
		virtual int GetSourceModuleCount() const;
		void SetSeed(unsigned int seed);
		void SetNoiseQuality(noise::NoiseQuality noiseQuality);

		void SetCraterCount(float craterCount);
		void SetCraterIntensity(float craterIntensity);
		void SetCraterScale(float craterScale);

	private:
		PgTerrainModule _terrainModule;

		PgCraterModule _craterModule;
		noise::module::Add _craterAdd;

		noise::module::Perlin _craterFreq;
		noise::module::ScaleBias _craterFreqBias;
		noise::module::Clamp _craterFreqClamp;
		noise::module::Multiply _craterFreqMult;

		noise::module::Const _flatOcean;
		noise::module::Select _blendOceanSmoothly;
		noise::module::ScaleBias _scaleResult;
		noise::module::Clamp _clampOcean;

		static const int _SourceModuleCount = 0;
	};
}

#endif//__PG_PLANET_MODULE__