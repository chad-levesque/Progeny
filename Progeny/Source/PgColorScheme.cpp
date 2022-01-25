#include "PgColorScheme.h"

#include "MathTools.h"

using namespace Progeny;

unsigned int
PgColorScheme::blendColor(float blendFactor, unsigned int color1, unsigned int color2)
{
	int red1 = (color1 & 0x00FF0000) >> 16;
	int red2 = (color2 & 0x00FF0000) >> 16;
	int newRed = (int)( red1 + blendFactor*(red2-red1) );
	clamp(newRed, 0, 0xFF);
	newRed = newRed << 16;
	int green1 = (color1 & 0x0000FF00) >> 8;
	int green2 = (color2 & 0x0000FF00) >> 8;
	int newGreen = (int)( green1 + blendFactor*(green2-green1) );
	clamp(newGreen, 0, 0xFF);
	newGreen = newGreen << 8;
	int blue1 = (color1 & 0x000000FF);
	int blue2 = (color2 & 0x000000FF);
	int newBlue = (int)( blue1 + blendFactor*(blue2-blue1) );
	clamp(newBlue, 0, 0xFF);
	int color = 0xFF000000 | newRed | newGreen | newBlue;
	return color;
}