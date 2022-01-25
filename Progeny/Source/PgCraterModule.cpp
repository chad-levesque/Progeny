#include "PgCraterModule.h"

using namespace Progeny;

PgCraterModule::PgCraterModule()
	: noise::module::Module(_SourceModuleCount),
	_ZeroConstant(),
	_VoronoiCells(),
	_ClampCells(),
	_ScaleCells(),
	_SelectEdgeFromBowl(),
	_InvertCraterEdge(),
	_ScaleCraterEdge(),
	_SelectEdgeOrBowl(),
	_ScaleCrater(),
	_CacheCells(),
	//These numbers derived by trial and error
	_BaseScale(1.3f),
	_BaseFrequency(0.50f)
{
	//The Ground Plane.
	_ZeroConstant.SetConstValue(0.0f);

	// Generate a set of cells
	_VoronoiCells.EnableDistance(true);
	_VoronoiCells.SetDisplacement(0.0001f);
	_VoronoiCells.SetFrequency(_BaseFrequency);

	// Cull shallow cells from the crater map
	float CellFrequency = 0.4f; //The depth to clamp cells at
	_ClampCells.SetSourceModule(0, _VoronoiCells);
	_ClampCells.SetBounds(-1.0f, -CellFrequency);

	// Move the plane up to ground level
	_ScaleCells.SetSourceModule(0, _ClampCells);
	_ScaleCells.SetBias(CellFrequency);

	_CacheCells.SetSourceModule(0, _ScaleCells);

	// Cut the Bottom off the crater for use as it's edge
	_SelectEdgeFromBowl.SetSourceModule(0, _ZeroConstant);
	_SelectEdgeFromBowl.SetSourceModule(1, _CacheCells);
	_SelectEdgeFromBowl.SetControlModule(_CacheCells);
	float EdgeWidth = 0.1f;
	_SelectEdgeFromBowl.SetBounds(-EdgeWidth, 0.0f);
	_SelectEdgeFromBowl.SetEdgeFalloff(0.3f); //The edge is sharp otherwise

	// Flip the edge over an move it up past the ground plane
	_InvertCraterEdge.SetSourceModule(0, _SelectEdgeFromBowl);
	float EdgeIntensityMultiplier = 3.0f; // The height of the crater edge (default intensity is 0.1)
	_ScaleCraterEdge.SetSourceModule(0, _InvertCraterEdge);
	_ScaleCraterEdge.SetScale(EdgeIntensityMultiplier);

	// Mix the crater bowl and the crater edge together.
	_SelectEdgeOrBowl.SetSourceModule(0, _CacheCells);
	_SelectEdgeOrBowl.SetSourceModule(1, _ScaleCraterEdge);
	_SelectEdgeOrBowl.SetControlModule(_ScaleCraterEdge);
	_SelectEdgeOrBowl.SetBounds(0.1f,1000.0f);
	_SelectEdgeOrBowl.SetEdgeFalloff(0.3f);

	//Scale the craters up in intensity
	_ScaleCrater.SetSourceModule(0, _SelectEdgeOrBowl);
	_ScaleCrater.SetScale(_BaseScale);
}

double PgCraterModule::GetValue(double x, double y, double z) const
{
	return _ScaleCrater.GetValue(x,y,z);
}

int PgCraterModule::GetSourceModuleCount() const
{
	return _SourceModuleCount;
}

void PgCraterModule::SetFrequency(float frequency)
{
	_VoronoiCells.SetFrequency(_BaseFrequency*frequency);
}

void PgCraterModule::SetScale(float scale)
{
	_ScaleCrater.SetScale(_BaseScale*scale);
}

void PgCraterModule::SetSeed(unsigned int seed)
{
	_VoronoiCells.SetSeed(seed);
}