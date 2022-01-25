#ifndef __PROGENY_COLOR_SCHEME_H__
#define __PROGENY_COLOR_SCHEME_H__
#include <stdlib.h>

#include <ctime>

namespace Progeny
{
	enum ColorScheme {
			COLORS_MERCURY,
			COLORS_VENUS,
			COLORS_EARTH,
			COLORS_MARS,
			COLORS_JUPITER,
			COLORS_SATURN,
			COLORS_URANUS,
			COLORS_NEPTUNE,
			COLORS_ASTEROID,
			COLORS_MOON,
			NUM_SCHEMES };
	class PgColorScheme 
	{
	public:
		PgColorScheme() { Earth(*this); }
		static void init(PgColorScheme& scheme, ColorScheme schemeClass)
		{
			switch(schemeClass)
			{
			case COLORS_MERCURY:
				return Mercury(scheme);
			case COLORS_VENUS:
				return Venus(scheme);
			case COLORS_EARTH:
				return Earth(scheme);
			case COLORS_MARS:
				return Mars(scheme);
			case COLORS_JUPITER:
				return Jupiter(scheme);
			case COLORS_SATURN:
				return Saturn(scheme);
			case COLORS_URANUS:
				return Uranus(scheme);
			case COLORS_NEPTUNE:
				return Neptune(scheme);
			case COLORS_ASTEROID:
				return Asteroid(scheme);
			case COLORS_MOON:
				return Moon(scheme);
			return Earth(scheme);
			}
		}
		
		static void RandomScheme(PgColorScheme& scheme)
		{
			srand((unsigned int)clock());
			int randomScheme = rand() % NUM_SCHEMES;
			return init(scheme, (ColorScheme)randomScheme);
		}
		static void Earth(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.7f;
			scheme.fRangeHeights[1] = 0.9f;
			scheme.fRangeHeights[2] = 1.0f;
			scheme.iRangeColors[0] = 0xFF2A2C48; // blue (water)
			scheme.iRangeColors[1] = 0xFF2E592E; // green (grass)
			scheme.iRangeColors[2] = 0xFF5D5D5D; // white (snow)
			scheme.fPolarLatitude =  0.6f; // polar circle latitude
			scheme.iSkyColor = 0xFF71B8EB; // light blue sky
		}
		static void Moon(PgColorScheme& scheme)
		{
			// TODO refine colors/ranges
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.3f;
			scheme.fRangeHeights[1] = 0.8f;
			scheme.fRangeHeights[2] = 1.0f;
			scheme.iRangeColors[0] = 0xFF828689; // dark grey (lunar mare)
			scheme.iRangeColors[1] = 0xFF969696; // moderate surface
			scheme.iRangeColors[2] = 0xFFb9b9b9; // lunar highlands
			scheme.fPolarLatitude =  1.0f; // polar circle latitude
			scheme.iSkyColor = 0x00000000; // no atmosphere
		}
		static void Mercury(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 2;
			scheme.fRangeHeights[0] = 0.5f;
			scheme.fRangeHeights[1] = 1.0f;
			scheme.iRangeColors[0] = 0xFF786248;
			scheme.iRangeColors[1] = 0xFF785f41;
			scheme.fPolarLatitude = 1.0f;
			scheme.iSkyColor = 0x00000000; // no atmosphere
		}
		static void Venus(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.5f;
			scheme.fRangeHeights[1] = 0.8f;
			scheme.fRangeHeights[2] = 1.0f; 
			scheme.iRangeColors[0] = 0xFF786248;
			scheme.iRangeColors[1] = 0xFF785f41;
			scheme.iRangeColors[2] = 0xFF78654e;
			scheme.fPolarLatitude = 1.0f;
			scheme.iSkyColor = 0xFF857455;
		}
		static void Mars(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.1f;
			scheme.fRangeHeights[1] = 0.8f;
			scheme.fRangeHeights[1] = 1.0f;
			scheme.iRangeColors[0] = 0xFF943118;
			scheme.iRangeColors[1] = 0xFF7b2918;
			scheme.iRangeColors[2] = 0xFF4a2918;
			scheme.fPolarLatitude = 1.0f;
			scheme.iSkyColor = 0xFFfeb87c;
		}
		static void Jupiter(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 4;
			scheme.fRangeHeights[0] = 0.2f;
			scheme.fRangeHeights[1] = 0.6f;
			scheme.fRangeHeights[2] = 0.9f;
			scheme.fRangeHeights[3] = 1.0f;
			scheme.iRangeColors[0] = 0xFFd5bfab;
			scheme.iRangeColors[1] = 0xFFbf7d4d;
			scheme.iRangeColors[2] = 0xFFc1a17a;
			scheme.iRangeColors[3] = 0xFF92785c;
			scheme.fPolarLatitude = 0.3f;
			scheme.iSkyColor = 0xFF966845;
		}
		static void Saturn(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.8f;
			scheme.fRangeHeights[1] = 0.9f;
			scheme.fRangeHeights[1] = 1.0f;
			scheme.iRangeColors[0] = 0xFFdabd9f;
			scheme.iRangeColors[1] = 0xFFb8a797;
			scheme.iRangeColors[2] = 0xFF857b71;
			scheme.fPolarLatitude =0.2f;
			scheme.iSkyColor = 0xFFc6a673;
		}
		static void Uranus(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.3f;
			scheme.fRangeHeights[1] = 0.6f;
			scheme.fRangeHeights[1] = 1.0f;
			scheme.iRangeColors[0] = 0xFF496980;
			scheme.iRangeColors[1] = 0xFF80c4d7;
			scheme.iRangeColors[2] = 0xFF9ee4ec;
			scheme.fPolarLatitude = 1.0f;
			scheme.iSkyColor = 0xFF00b1ff;
		}
		static void Neptune(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 3;
			scheme.fRangeHeights[0] = 0.6f;
			scheme.fRangeHeights[1] = 0.8f;
			scheme.fRangeHeights[1] = 1.0f;
			scheme.iRangeColors[0] = 0xFF6348a7;
			scheme.iRangeColors[1] = 0xFF9474f9;
			scheme.iRangeColors[2] = 0xFF9892fc;
			scheme.fPolarLatitude = 0.4f;
			scheme.iSkyColor = 0xFF4c6aa8;
		}
		static void Asteroid(PgColorScheme& scheme)
		{
			scheme.nNumRanges = 4;
			scheme.fRangeHeights[0] = 0.1f;
			scheme.fRangeHeights[1] = 0.3f;
			scheme.fRangeHeights[2] = 0.6f;
			scheme.fRangeHeights[3] = 1.0f;
			scheme.iRangeColors[0] = 0xFF342a1e;
			scheme.iRangeColors[1] = 0xFF28221a;
			scheme.iRangeColors[2] = 0xFF353029;
			scheme.iRangeColors[3] = 0xFF363432;
			scheme.fPolarLatitude = 1.0f;
			scheme.iSkyColor = 0x00000000;
		}
		
		static unsigned int blendColor(float blendFactor, unsigned int color1, unsigned int color2);
		int nNumRanges;
		float fRangeHeights[8];
		unsigned int iRangeColors[8];
		float fPolarLatitude;
		unsigned int iSkyColor;
	};
}

#endif