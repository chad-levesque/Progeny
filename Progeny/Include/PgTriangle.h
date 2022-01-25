#ifndef __PROGENY_PGTRIANGLE_H__
#define __PROGENY_PGTRIANGLE_H__

#include "LinkedList.h"
#include "PgVector3.h"
#include "PgVertex.h"

namespace Progeny {
	class PgDiamond;
	class IPgCamera;
	#define PgTriangleList LinkedList<PgTriangle>
	class PgTriangle : public LinkedListNode<PgTriangle>
	{
	public:
		// Triangle members
		unsigned short _vertex[3];	// Indices into a PgVertexArray
		PgVector3 _normal;			// The normal of the triangle (used for flat shading)
		PgTriangle* _edge[3];		// Pointers to the triangles sharing edges with this one
		PgTriangle* _parent;		// Pointer to the parent triangle that created this one
		PgDiamond* _diamond;		// Pointer to the diamond that contains this triangle, if a diamond contains this triangle

		// Split priority members
		PgVector3 _midpoint;		// The midpoint of the third edge (the one to split)
		float _offset;				// The offset in height the midpoint will require when split
		float _length;			// The length of the third edge (the one to split)
		int _waitCounter;

	// Operations
	public:
		PgTriangle();
		PgTriangle(unsigned short n1, unsigned short n2, unsigned short n3);

		PgVertex* getVertex (unsigned short n);

		void setVertices(unsigned short n1, unsigned short n2, unsigned short n3);
		void setEdges( PgTriangle *p1, PgTriangle *p2, PgTriangle *p3 );
		void replaceEdge( PgTriangle *p1, PgTriangle *p2);
		bool hasVertex(unsigned short index);

		float getPriority(IPgCamera* pCamera, float horizon);
		void copyPriority(PgTriangle* p);
		void computeNormal();
		void updateVertexNormals();
		inline int getWaitCounter() const { return _waitCounter; }
		inline void setWaitCounter(int c) { _waitCounter = c; }
		
		void draw(unsigned short *nArray, unsigned int &nIndex);
	};
}

#endif