#ifndef __PROGENY_SPHERE_H__
#define __PROGENY_SPHERE_H__

#include "Mesh.h"
#include "PgSceneObject.h"
#include <libnoise/noise.h>
#include "noiseutils.h"

namespace Progeny
{
	class PgSphere : public Mesh, public PgSceneObject
	{
	public:
		PgSphere();
		PgSphere( double radius, int divisions, double elevationVariation, bool randomColors = false);
		virtual ~PgSphere();
		const char* getTextureLocation() const { return _textureLocation; }
	private:
		void init();
		void generateMesh();
		void addVertex(double x, double y, double z, double u, double v);
		void addTriangle(int ind1, int ind2, int ind3);
		void generateTexture();
		
		double _radius;
		int _divisions;
		double _elevationVariation;
		bool _randomColors;

		double _stepSize;
		int _stripLength;
		int _numberOfStrips;
		const char* _textureLocation;
		noise::module::Perlin _noiseModule;
		noise::utils::NoiseMap _noiseMap;
	};
}

#endif //__PROGENY_SPHERE_H__