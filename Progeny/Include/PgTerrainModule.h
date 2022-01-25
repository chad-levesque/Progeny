#ifndef __PG_TERRAIN_MODULE__
#define __PG_TERRAIN_MODULE__

#include "libnoise/../../src/module/modulebase.h" //This feels wrong... but oh well
#include "libnoise/noise.h"

namespace Progeny
{
	struct PgPlanetAttributes;

	/** Progeny implementation of a terrain noise module
	 * This class inherits from a class internal to libnoise
	 * Should probably reside within a libnoise extension, but
	 * should be fine here for now.
	 */
	class PgTerrainModule : public noise::module::Module
	{
	public:
		PgTerrainModule(const PgPlanetAttributes& _attribues);
		virtual double GetValue (double x, double y, double z) const;
		virtual int GetSourceModuleCount() const;
		void SetSeed(unsigned int seed);
		void SetNoiseQuality(noise::NoiseQuality noiseQuality);

		void SetMountainHeight(float mountainHeight);

	private:
		noise::module::Perlin _continentMask;
		noise::module::RidgedMulti _landMountians;
		noise::module::ScaleBias _mountianBias;
		noise::module::Select _hillsOrMountians;
		noise::module::Max _mountianWithoutDips;
		noise::module::Cache _continentMaskCached;
		noise::module::Clamp _continentMaskClamped;

		//high frequency noise 
		noise::module::Perlin _highFrequencyNoise;
		noise::module::ScaleBias _highFrequencyNoiseBias;
		noise::module::Add _addHighFrequencyNoise;

		//No required source modules
		static int const _SourceModuleCount = 0;
	};
}

#endif //__PG_PLANET_MODULE__