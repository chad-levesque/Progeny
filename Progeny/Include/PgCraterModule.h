#ifndef __PG_CRATER_MODULE__
#define __PG_CRATER_MODULE__

#include "libnoise/../../src/module/modulebase.h" //This feels wrong... but oh well
#include "libnoise/noise.h"

namespace Progeny
{
	/** Progeny implementation of a crater noise module 
	 * This class inherits from a class internal to libnoise
	 * Should probably reside within a libnoise extension, but
	 * should be fine here for now.
	 */
	class PgCraterModule : public noise::module::Module
	{
	public:
		PgCraterModule();
		virtual double GetValue (double x, double y, double z) const;
		virtual int GetSourceModuleCount() const;
		void SetSeed(unsigned int seed);
		void SetScale(float scale);
		void SetFrequency(float frequency);

	private:
		noise::module::Const _ZeroConstant;
		noise::module::Voronoi _VoronoiCells;
		noise::module::Clamp _ClampCells;
		noise::module::ScaleBias _ScaleCells;
		noise::module::Select _SelectEdgeFromBowl;
		noise::module::Invert _InvertCraterEdge;
		noise::module::ScaleBias _ScaleCraterEdge;
		noise::module::Select _SelectEdgeOrBowl;
		noise::module::ScaleBias _ScaleCrater;
		noise::module::Cache _CacheCells;
		
		const float _BaseScale;
		const float _BaseFrequency;

		//No required source modules
		static int const _SourceModuleCount = 0;
	};
}

#endif //__PG_CRATER_MODULE__