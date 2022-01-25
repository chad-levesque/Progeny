#ifndef __PROGENYIPGRENDERABLE_H__
#define __PROGENYIPGRENDERABLE_H__

namespace Progeny
{
	class IPgRenderer;

	class IPgRenderable
	{
	public:
		virtual ~IPgRenderable() { }
		virtual void render(IPgRenderer* renderer) = 0;
	};
}

#endif