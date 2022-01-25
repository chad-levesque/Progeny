#ifndef __PROGENY_DIAMOND_H__
#define __PROGENY_DIAMOND_H__

#include "LinkedList.h"
#include "PgVector3.h"

namespace Progeny
{
	class PgTriangle;
	class IPgCamera;

	#define PgDiamondList LinkedList<PgDiamond>
	class PgDiamond : public LinkedListNode<PgDiamond>
	{
	public:
		PgTriangle* _parent[2];
		PgTriangle* _child[2];

		PgVector3 _midpoint;
		PgVector3 _origMidpoint;
		float _offset;
		float _length;

	public:
		PgDiamond(PgTriangle* parent1, PgTriangle* parent2, PgTriangle* child1, PgTriangle* child2 );
		//virtual ~PgDiamond();
		float getPriority( IPgCamera* pCamera, float horizon );
		bool hasTriangle(PgTriangle* triangle);
		inline int getWaitCounter() const { return _waitCounter; }
		inline void setWaitCounter(int c) { _waitCounter = c; }
	private:
		int _waitCounter;

	};
}

#endif