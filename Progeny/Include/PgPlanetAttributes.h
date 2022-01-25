#ifndef __PROGENY_PLANET_ATTRUBUTES_H__
#define __PROGENY_PLANET_ATTRUBUTES_H__

namespace Progeny
{
	struct PgPlanetAttributes
	{
		unsigned int _seed; // Noise seed value -> Any Value is Safe
		float _radius; // size of planet -> 2,500 - 10,000 safe values
		float _oceanHeight; // Ratio of ocean to land -> 0-1 safe values
		float _mountainCount; // Frequency of mountains -> 0-1 safe values
		float _continentFreq; // Frequency of continents -> 0-1 safe values
		float _noiseAmplitude; // How hilly can we make the planet
		bool _useCraters; // Has the planet been hit by other world objects?
	};
}

#endif