#ifndef __PROGENY_IPGCAMERA_H__
#define __PROGENY_IPGCAMERA_H__

#include <PgVector3.h>

namespace Progeny
{
	class IPgCamera
	{
	public:
		virtual Progeny::PgVector3 getCameraPosition( ) = 0;
		virtual Progeny::PgVector3 getCameraHeading() = 0;
	};
}

#endif