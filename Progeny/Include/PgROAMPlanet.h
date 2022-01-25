#ifndef __PROGENY_PGROAMPLANET_H__
#define __PROGENY_PGROAMPLANET_H__

#include "IPgRenderable.h"

#include "LinkedList.h"
#include "PgTriangle.h"
#include "PgDiamond.h"

namespace Progeny
{
	class PgPlanetStrategy;
	class IPgRenderer;
	class IPgCamera;

	class PgROAMPlanet : public IPgRenderable
	{
	public:

		PgROAMPlanet(PgPlanetStrategy* pPlanetStrategy);

		~PgROAMPlanet()
		{
		}
		void init();
		bool update( IPgCamera* camera, float tolerance );
		void render(IPgRenderer* renderer);
		int getTriangleCount();
		float getAltitude(const PgVector3& position);
	protected:
		LinkedList<PgTriangle> _triangleList;
		LinkedList<PgDiamond> _diamondList;
		void rebuildEdges();
		bool split( PgTriangle *pTriangle );
		void initializeBase();
		void updateTriangleOffset(PgTriangle* pTriangle);
		void merge(PgDiamond* pDiamond);
		void mergeAll()
		{
			while(!_diamondList.IsEmpty())
				merge(_diamondList.GetHead());
		}
		float _cameraPos[3];

	private:
		const PgPlanetStrategy* _planetStrategy;
		int _triCount;
	};
}
#endif // __PROGENY_PGROAMPLANET_H__