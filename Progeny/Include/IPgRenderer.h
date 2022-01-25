#ifndef __PROGENYIPGRENDERER_H__
#define __PROGENYIPGRENDERER_H__

namespace Progeny
{
	class PgVertex;

	class IPgRenderer
	{
	public:
		virtual ~IPgRenderer() { }
		virtual void drawIndexedPrimitives(unsigned short* iArray, PgVertex* vArray, int triangleCount) = 0;
	};
}

#endif