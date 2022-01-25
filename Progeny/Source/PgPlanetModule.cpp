#include "PgPlanetModule.h"
#include "PgPlanetAttributes.h"

#include "noiseutils.h"
#include "PgConstants.h"

using namespace Progeny;

PgPlanetModule::PgPlanetModule(const PgPlanetAttributes& attributes)
	: noise::module::Module(_SourceModuleCount),
	_terrainModule(attributes),
	_craterModule(),
	_craterAdd(),
	_craterFreq(),
	_craterFreqBias(),
	_craterFreqClamp(),
	_craterFreqMult(),
	_flatOcean(),
	_blendOceanSmoothly(),
	_scaleResult(),
	_clampOcean()
{
	// Convert an OceanHeight of 0-1 to -1 to 1 for use with noise.
	float noiseOceanHeight = 2.0f*(attributes._oceanHeight-0.5f) - 0.001; //0.001 added to deal with inaccurate floats
	float mountainCutoff = (1-attributes._oceanHeight)*attributes._mountainCount; //Mountain count is a percentage of remaining land.
	mountainCutoff = 1-mountainCutoff; // we want to convert the count into a cutoff height (invert it).
	mountainCutoff = 2.0f*(mountainCutoff-0.5f); //final conversion to a cutoff height.

	_craterFreq.SetFrequency(2.0f);
	_craterFreqBias.SetSourceModule(0, _craterFreq);
	_craterFreqBias.SetScale(0.5f);
	_craterFreqBias.SetBias(0.5f); //initial setting, this can be adjusted by the SetCraterCount function.
	_craterFreqClamp.SetSourceModule(0, _craterFreqBias);
	_craterFreqClamp.SetBounds(0.0f,1.0f);
	_craterFreqMult.SetSourceModule(0, _craterFreqClamp);
	_craterFreqMult.SetSourceModule(1, _craterModule);

	// Add craters to the mix.
	_craterAdd.SetSourceModule(0, _terrainModule);
	_craterAdd.SetSourceModule(1, _craterModule);

	// The main ocean should be at zero as well.
	_flatOcean.SetConstValue(noiseOceanHeight);
	// This module is purely for blending the edges, otherwise we'd just do a clamp.
	_blendOceanSmoothly.SetSourceModule(0, _flatOcean);

	//TODO: Implement crater count
	if(attributes._useCraters)
	{
		_blendOceanSmoothly.SetSourceModule(1, _craterAdd);
		_blendOceanSmoothly.SetControlModule(_craterAdd);
	}
	else
	{
		_blendOceanSmoothly.SetSourceModule(1, _terrainModule);
		_blendOceanSmoothly.SetControlModule(_terrainModule);
	}

	_blendOceanSmoothly.SetBounds(noiseOceanHeight, 1000.0f);
	_blendOceanSmoothly.SetEdgeFalloff(0.15f);

	//Clamp the ocean so that no dips or bumps appear from the select modules smoothing process.
	_clampOcean.SetSourceModule(0, _blendOceanSmoothly);
	_clampOcean.SetBounds(noiseOceanHeight, 1000.0f);

	float finalScale = 1.0f;
	_scaleResult.SetSourceModule(0, _clampOcean);
	_scaleResult.SetScale(finalScale);

}

double PgPlanetModule::GetValue(double x, double y, double z) const
{
	return _scaleResult.GetValue(x,y,z);
}

int PgPlanetModule::GetSourceModuleCount() const 
{
	return _SourceModuleCount;
}

void PgPlanetModule::SetSeed(unsigned int seed)
{
	_terrainModule.SetSeed(seed+0);
	_craterModule.SetSeed(seed+1);
	_craterFreq.SetSeed(seed+2);

	#if (OUTPUT_NOISES_MODULES_TO_FILE)
	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderSphere heightMapBuilder;
	noise::utils::RendererImage renderer;
	noise::utils::Image image;
	noise::utils::WriterBMP writer;

	// Output terrain:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_terrainModule);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_terrainModule.bmp");
	writer.WriteDestFile ();

	// Output crater module:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_craterModule);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_craterModule.bmp");
	writer.WriteDestFile ();
	
	// Output crater freq:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_craterFreq);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_craterFreq.bmp");
	writer.WriteDestFile ();

	
	// Output final scaled terrain:
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.SetSourceModule (_scaleResult);
	heightMapBuilder.Build ();
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();
	writer.SetSourceImage (image);
	writer.SetDestFilename ("_scaleResult.bmp");
	writer.WriteDestFile ();
	#endif //OUTPUT_NOISES_MODULES_TO_FILE

}

void PgPlanetModule::SetNoiseQuality(noise::NoiseQuality noiseQuality)
{
	_terrainModule.SetNoiseQuality(noiseQuality);
	_craterFreq.SetNoiseQuality(noiseQuality);
}

void PgPlanetModule::SetCraterCount(float craterCount)
{
	//moves the selection Perlin noise up or down, thus increasing or decreasing the influence of craters.
	_craterFreqBias.SetBias(craterCount);
}

void PgPlanetModule::SetCraterIntensity(float craterIntensity)
{
	_craterModule.SetScale(craterIntensity);
}

void PgPlanetModule::SetCraterScale(float craterScale)
{
	_craterModule.SetFrequency(craterScale);
}