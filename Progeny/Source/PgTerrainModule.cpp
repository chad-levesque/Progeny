#include "PgTerrainModule.h"
#include "PgPlanetAttributes.h"

#include "noiseutils.h"
#include "PgConstants.h"

using namespace Progeny;

PgTerrainModule::PgTerrainModule(const PgPlanetAttributes& _attributes)
	: noise::module::Module(_SourceModuleCount),
	_continentMask(),
	_landMountians(),
	_mountianBias(),
	_hillsOrMountians(),
	_mountianWithoutDips(),
	_continentMaskCached(),
	_continentMaskClamped()
{
	// Convert an OceanHeight of 0-1 to -1 to 1 for use with noise.
	float noiseOceanHeight = 2.0f*(_attributes._oceanHeight-0.5f) - 0.001; //0.001 added to deal with inaccurate floats
	float mountainCutoff = (1-_attributes._oceanHeight)*_attributes._mountainCount; //Mountain count is a percentage of remaining land.
	mountainCutoff = 1-mountainCutoff; // we want to convert the count into a cutoff height (invert it).
	mountainCutoff = 2.0f*(mountainCutoff-0.5f); //final conversion to a cutoff height.

	// The mask for defining the position of continents
	_continentMask.SetOctaveCount(1);
	_continentMask.SetFrequency(_attributes._continentFreq+0.1f);
	_continentMaskCached.SetSourceModule(0, _continentMask);
	// We clamp our control modules so we can sanely work with -1 to 1 values.
	_continentMaskClamped.SetSourceModule(0, _continentMaskCached);
	_continentMaskClamped.SetBounds(-0.9999f,0.9999f);

	// Mountains ----------------------------------------------------------------------------
	// The rigid Noise module used to define the landmass's mountain areas.
	_landMountians.SetOctaveCount(4);
	_landMountians.SetFrequency(1.0f);

	_mountianBias.SetSourceModule(0, _landMountians);
	float mountianLevel = 0.55f;
	_mountianBias.SetScale(mountianLevel);
	_mountianBias.SetBias(noiseOceanHeight+0.25f);

	// take max of land and mountains so that we don't get huge dips that the rigid multi-fractals would add.
	_mountianWithoutDips.SetSourceModule(0, _mountianBias);
	_mountianWithoutDips.SetSourceModule(1, _continentMask);
	//--------------------------------------------------------------------------------------

	// Blend hills and mountains specified amount.
	_hillsOrMountians.SetSourceModule(0, _continentMaskCached);
	_hillsOrMountians.SetSourceModule(1, _mountianWithoutDips);
	_hillsOrMountians.SetControlModule(_continentMaskClamped);
	_hillsOrMountians.SetBounds(mountainCutoff, 1000.0f);
	_hillsOrMountians.SetEdgeFalloff(0.08f);

	//new, testing out billows/high frequency noise.
	_highFrequencyNoise.SetFrequency(3.5);
	_highFrequencyNoise.SetOctaveCount(3);
	_highFrequencyNoiseBias.SetSourceModule(0, _highFrequencyNoise);
	_highFrequencyNoiseBias.SetScale(0.15f);

	//Alternate between flat and high frequency areas
	_addHighFrequencyNoise.SetSourceModule(0, _highFrequencyNoiseBias);
	_addHighFrequencyNoise.SetSourceModule(1, _hillsOrMountians);
}

double PgTerrainModule::GetValue(double x, double y, double z) const
{
	return _addHighFrequencyNoise.GetValue(x,y,z);
}

int PgTerrainModule::GetSourceModuleCount() const
{
	return _SourceModuleCount;
}

void PgTerrainModule::SetSeed(unsigned int seed)
{
	_continentMask.SetSeed(seed);
	_landMountians.SetSeed(seed);
	_highFrequencyNoise.SetSeed(seed);

	#if (OUTPUT_NOISES_MODULES_TO_FILE)
	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderSphere heightMapBuilder;
	noise::utils::RendererImage renderer;
	noise::utils::Image image;
	noise::utils::WriterBMP writer;

	// Output _continentMask:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_continentMask);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_continentMask.bmp");
	writer.WriteDestFile ();

	// Output _landMountians:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_landMountians);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_landMountians.bmp");
	writer.WriteDestFile ();
	
	// Output _highFrequencyNoise:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_highFrequencyNoise);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_highFrequencyNoise.bmp");
	writer.WriteDestFile ();
	#endif // (OUTPUT_NOISES_MODULES_TO_FILE)
}

void PgTerrainModule::SetNoiseQuality(noise::NoiseQuality noiseQuality)
{
	_continentMask.SetNoiseQuality(noiseQuality);
	_landMountians.SetNoiseQuality(noiseQuality);
	_highFrequencyNoise.SetNoiseQuality(noiseQuality);
}

void PgTerrainModule::SetMountainHeight(float mountainHeight)
{

}