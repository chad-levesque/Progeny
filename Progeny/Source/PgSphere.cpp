#include "PgSphere.h"
#include "Vector3.h"
#include "Vertex.h"
#include "MathTools.h"

using namespace Progeny;

#include <libnoise/noise.h>
#include "noiseutils.h"

int const TEXTURE_WIDTH = 1024;
int const TEXTURE_HEIGHT = 512;
char* TEXTURE_LOCATION = "heightmap.bmp";

PgSphere::PgSphere() :
	_radius( 1.0 ),
	_divisions( 10 )
{
	init();
}

PgSphere::PgSphere(double radius, int divisions, double elevationVariation, bool randomColors) :
	_radius(radius),
	_divisions(divisions),
	_elevationVariation(elevationVariation),
	_randomColors(randomColors)
{
	init();
}

void PgSphere::init()
{
	_stepSize = PI/_divisions;
	_stripLength = 2 * _divisions + 1;
	_numberOfStrips = _divisions - 1;
	generateTexture();
	generateMesh();
}

PgSphere::~PgSphere()
{
}

void PgSphere::generateMesh()
{
	double beta = _stepSize;
	int divisionsPerLatitude = 2 * _divisions;

	addVertex( 0.0, _radius, 0.0, .5, 0.0 );
	for ( int j = 1; j < _divisions; j++ )
	{
		double alpha = 0.0;
		for ( int i = 0; i <= divisionsPerLatitude; i++ )
		{
			double x = cos( alpha ) * sin( beta );
			double y = cos( beta );
			double z = sin( alpha ) * sin( beta );
			double height = _noiseModule.GetValue(x, y, z) * (_radius*_elevationVariation) + _radius;
			x *= height;
			y *= height;
			z *= height;

			double v = ((double)j / (double)_divisions);
			double u = ((double)i / (double)divisionsPerLatitude);

			addVertex( x, y, z, u, v );

			alpha += _stepSize;
		}
		beta += _stepSize;
	}
	addVertex( 0.0, -_radius, 0.0, 0.5, 1.0 );

	//Create triangles from points
	int lastVertexNumber = getVertexCount() - 1;

	// Zenith(top) triangles
	for ( int i = 1; i < _stripLength; i++ )
	{
		addTriangle( i, 0, i+1 );
	}

	// Strip triangles
	if ( _numberOfStrips > 1 )
	{
		for (int stripNum = 1; stripNum < _numberOfStrips; stripNum++ )
		{
			int stripStart = 1 + ( ( stripNum - 1 ) * _stripLength );
			int stripEnd = stripStart + _stripLength - 1;
			for ( int i = stripStart; i < stripEnd; i++ )
			{
				addTriangle( i,  i+1, i+1+_stripLength );
				addTriangle( i,  i+1+_stripLength, i+_stripLength );
			}
		}
	}
	
	// Nadir(bottom) triangles
	for ( int i = lastVertexNumber - _stripLength; i < lastVertexNumber - 1; i++ )
	{
		addTriangle(i, i+1, lastVertexNumber );
	}
}

void PgSphere::addVertex(double x, double y, double z, double u, double v) 
{
	Vector3 position(x,y,z);
	Vertex* vertex = new Vertex(position);

	//map texture coordinate using spherical coordinates
	//double radius = sqrt(x*x + y*y + z*z);

	vertex->setU(u);
	vertex->setV(v);

	Mesh::addVertex(vertex);
}

void PgSphere::addTriangle(int ind1, int ind2, int ind3) 
{
	Mesh::addTriangle(ind1, ind2, ind3);
}

void PgSphere::generateTexture() {
	_noiseModule.SetSeed( rand() );
	utils::NoiseMapBuilderSphere heightMapBuilder;
	heightMapBuilder.SetSourceModule (_noiseModule);
	heightMapBuilder.SetDestNoiseMap (_noiseMap);
	heightMapBuilder.SetDestSize (TEXTURE_WIDTH, TEXTURE_HEIGHT);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.Build();

	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap (_noiseMap);
	renderer.SetDestImage (image);
	renderer.ClearGradient ();
	if (_randomColors) {
		renderer.AddGradientPoint (-1.0000, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint (-0.2500, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 0.0000, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 0.0625, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 0.1250, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 0.3750, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 0.7500, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
		renderer.AddGradientPoint ( 1.0000, utils::Color ( 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255*(int)(double)rand()/(double)RAND_MAX, 255));
	} else {
		renderer.BuildTerrainGradient();
	}
	renderer.EnableLight();
	renderer.SetLightContrast (3.0); // Triple the contrast
	renderer.SetLightBrightness (2.0); // Double the brightness
	renderer.Render ();

	utils::WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename (TEXTURE_LOCATION);
	writer.WriteDestFile ();
	_textureLocation = TEXTURE_LOCATION;

}