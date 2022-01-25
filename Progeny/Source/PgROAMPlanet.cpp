#include "PgROAMPlanet.h"

#include "PgVertex.h"
#include "IPgRenderer.h"
#include "PgTriangle.h"
#include "PgDiamond.h"
#include "IPgCamera.h"
#include "PgConstants.h"
#include "PgPlanetStrategy.h"
#include "MathTools.h"

#include <ctime>

using namespace Progeny;

PgROAMPlanet::PgROAMPlanet(PgPlanetStrategy* pPlanetStrategy)
	: _triangleList(),
	_diamondList(),
	_planetStrategy(pPlanetStrategy),
	_triCount(0)
{

}

void
PgROAMPlanet::init()
{
	initializeBase();
}

bool
PgROAMPlanet::update( IPgCamera* camera, float tolerance )
{
	bool lockTolerance = false;
	PgVector3 cameraPosition = camera->getCameraPosition();

	// Get the horizon for the UpdatePosition() function
	float height = maxf(200.0f, cameraPosition.magnitude() - _planetStrategy->getBaseRadius() );
	float horizon = height*height + 2.0f*height*_planetStrategy->getBaseRadius();	// Distance to horizon squared

	float twiceTolerance = 2.0f*tolerance;
	float halfTolerance = 0.5f*tolerance;

	long startTime = clock();
	// Merge diamonds as appropriate:
	for ( PgDiamond* pDiamond = _diamondList.GetHead(); pDiamond->IsInList(); pDiamond = pDiamond->GetNext() )
	{
		if (clock() - startTime > MAX_MERGE_TIME)
		{
			lockTolerance = true;
			break;
		}
		if (pDiamond->getWaitCounter() > 0)
		{
			pDiamond->setWaitCounter( pDiamond->getWaitCounter() - 1 );
		}
		else
		{
			float priority = pDiamond->getPriority( camera, 1.1f*horizon );
			if ( priority < tolerance )
			{
				PgDiamond* pOldDiamond = pDiamond;
				pDiamond = pDiamond->GetPrevious();
				merge(pOldDiamond);
			}
			else if (priority > twiceTolerance)
			{
				pDiamond->setWaitCounter(4);
			}
			else
			{
				pDiamond->setWaitCounter(2);
			}
		}
	}
	
	// Split triangles as appropriate:
	bool keepSplitting = true;
	startTime = clock();
	bool splitHappened = false;
	for ( PgTriangle* pTriangle = _triangleList.GetHead(); pTriangle->IsInList() && keepSplitting && _triCount < MAX_NUMBER_TRIANGLES; pTriangle = pTriangle->GetNext() )
	{
		if (clock() - startTime > MAX_SPLIT_TIME)
		{
			lockTolerance = true;
			break;
		}
		if (pTriangle->getWaitCounter() > 0)
		{
			pTriangle->setWaitCounter( pTriangle->getWaitCounter() - 1 );
		}
		else
		{
			float priority = pTriangle->getPriority( camera, 1.1f*horizon );
			if (priority > tolerance)
			{
				keepSplitting = split(pTriangle);
				splitHappened = true;
			}
			else if (priority < EPSILON)
			{
				pTriangle->setWaitCounter(4);
			}
			else if (priority < halfTolerance)
			{
				pTriangle->setWaitCounter(2);
			}
			else
			{
				pTriangle->setWaitCounter(1);
			}
		}		
	}
	if (!splitHappened) return true;
	else return lockTolerance;
}

void
PgROAMPlanet::render(IPgRenderer* renderer)
{
	_triCount = _triangleList.GetCount();
	static unsigned short nArray[65535*3]; 
	static unsigned int nIndex;
	nIndex = 0;
	for(PgTriangle* p = _triangleList.GetHead(); p->IsInList(); p = p->GetNext())
		p->draw(nArray, nIndex);
	
	renderer->drawIndexedPrimitives(
		nArray, // index array
		PgVertex::Array[0], // vertex array
		_triCount // number of triangles
		);

}

float
PgROAMPlanet::getAltitude(const PgVector3& position)
{
	float elevation;
	unsigned int color;
	_planetStrategy->getSurfaceData( position, elevation, color );
	float altitude = position.magnitude()- elevation;
	return altitude;
}

int
PgROAMPlanet::getTriangleCount()
{
	// Returns the number of triangles from last render.
	return _triCount;
}

void
PgROAMPlanet::initializeBase( )
{
	#define CUBE_VERTICES			8
	#define CUBE_TRIANGLES			12
	float dist = 1.0f/sqrtf(3.0f);
	float fCube[CUBE_VERTICES*3] = {-dist,  dist,  dist,
									 dist,  dist,  dist,
									 dist, -dist,  dist,
									-dist, -dist,  dist,
									-dist,  dist, -dist,
									 dist,  dist, -dist,
									 dist, -dist, -dist,
									-dist, -dist, -dist };
	int nCube[CUBE_TRIANGLES*3] = { 1, 0, 3,
									3, 2, 1,
									5, 6, 7,
									7, 4, 5,
									0, 1, 5,
									5, 4, 0,
									3, 7, 6,
									6, 2, 3,
									0, 4, 7,
									7, 3, 0,
									1, 2, 6,
									6, 5, 1};

	_triangleList.RemoveAll();
	_diamondList.RemoveAll();

	unsigned short *nVertex = new unsigned short[CUBE_VERTICES];

	PgVertex** pVertex = new PgVertex*[CUBE_VERTICES];
	for(int i=0, j=0; i<CUBE_VERTICES; i++, j+=3)
	{
		PgVertex::Array.acquireVertex( nVertex[i] );
		float elevation;
		unsigned int color;
		_planetStrategy->getSurfaceData(PgVector3(fCube[j], fCube[j+1], fCube[j+2]), elevation, color );
		PgVector3 position = PgVector3(fCube[j], fCube[j+1], fCube[j+2])*elevation;
		PgVertex::Array[nVertex[i]]->Init( position );
		PgVertex::Array[nVertex[i]]->_vertData.color = color;
		
	}
	for(int i=0, j=0; i<CUBE_TRIANGLES; i++, j+=3)
	{
		_triangleList.AddTail(new PgTriangle(nVertex[nCube[j]], nVertex[nCube[j+1]], nVertex[nCube[j+2]]));
		++_triCount;
	}
	delete nVertex;
	rebuildEdges();
	for(PgTriangle* pTriangle = _triangleList.GetHead(); pTriangle->IsInList(); pTriangle = pTriangle->GetNext())
		updateTriangleOffset(pTriangle);
}

bool
PgROAMPlanet::split(PgTriangle *pTriangle)
{
	PgTriangle* pOpposite;
	if(pTriangle->_edge[2]->_edge[2] != pTriangle)
	{
		if ( split(pTriangle->_edge[2]) == false)
		{
			return false;
		}
	}
	pOpposite = pTriangle->_edge[2];

	static unsigned short nNewVertex;
	static PgVertex* pNewVertex;

	// Create a new vertex located at the center of the triangle's edge 2
	PgVector3 v = pTriangle->getVertex(0)->_vertData.position.midpoint( pTriangle->getVertex(2)->_vertData.position);
	v.normalize();
	if ( PgVertex::Array.acquireVertex(nNewVertex) == false )
	{
		return false; // Couldn't split.. out of available vertices!
	}
	pNewVertex = PgVertex::Array[nNewVertex];
	pNewVertex->Init(v);

	// Grab the current offset value for the triangle
	float offset = pTriangle->_offset;

	// Set height of new vertex
	Progeny::PgVector3 vertPos = pNewVertex->_vertData.position;
	float elevation;
	unsigned int color;
	_planetStrategy->getSurfaceData(vertPos, elevation, color);
	pNewVertex->_vertData.color = color;
	pNewVertex->_vertData.position *= elevation;

	// Create new triangles for split
	static PgTriangle *pNewTriangle[2];
	pNewTriangle[0] = new PgTriangle();
	pNewTriangle[1] = new PgTriangle();  

	// Split first one from pTriangle
	pNewTriangle[0]->InsertAfter(pTriangle);
	++_triCount;
	pNewTriangle[0]->setVertices(pTriangle->_vertex[2], nNewVertex, pTriangle->_vertex[1]);
	pTriangle->setVertices(pTriangle->_vertex[1], nNewVertex, pTriangle->_vertex[0]);
	pNewTriangle[0]->setEdges(pOpposite, pTriangle, pTriangle->_edge[1]);
	pTriangle->_edge[1]->replaceEdge(pTriangle, pNewTriangle[0]);
	pTriangle->setEdges( pNewTriangle[0], pNewTriangle[1], pTriangle->_edge[0] );
	// Set up priority members of first new triangle
	if(pNewTriangle[0]->_edge[2]->_edge[2] == pNewTriangle[0])
	{
		pNewTriangle[0]->copyPriority(pNewTriangle[0]->_edge[2]);
	}
	else
	{
		updateTriangleOffset(pNewTriangle[0]);
	}
	// and update the priority of the adjusted parent
	if(pTriangle->_edge[2]->_edge[2] == pTriangle)
	{
		pTriangle->copyPriority(pTriangle->_edge[2]);
	}
	else
	{
		updateTriangleOffset(pTriangle);
	}

	// Split second triangle
	pNewTriangle[1]->InsertAfter(pOpposite);
	++_triCount;
	pNewTriangle[1]->setVertices(pOpposite->_vertex[2], nNewVertex, pOpposite->_vertex[1]);
	pOpposite->setVertices(pOpposite->_vertex[1], nNewVertex, pOpposite->_vertex[0]);
	pNewTriangle[1]->setEdges(pTriangle, pOpposite, pOpposite->_edge[1]);
	pOpposite->_edge[1]->replaceEdge(pOpposite, pNewTriangle[1]);
	pOpposite->setEdges( pNewTriangle[1], pNewTriangle[0], pOpposite->_edge[0] );
	// Set up priority members of second new triangle
	if(pNewTriangle[1]->_edge[2]->_edge[2] == pNewTriangle[1])
	{
		pNewTriangle[1]->copyPriority(pNewTriangle[1]->_edge[2]);
	}
	else
	{
		updateTriangleOffset(pNewTriangle[1]);
	}
	if(pOpposite->_edge[2]->_edge[2] == pOpposite)
	{
		pOpposite->copyPriority(pOpposite->_edge[2]);
	}
	else
	{
		updateTriangleOffset(pOpposite);
	}

	pTriangle->updateVertexNormals();
	pOpposite->updateVertexNormals();

	// Set up parent pointers for the new triangles
	pNewTriangle[0]->_parent = pTriangle;
	pNewTriangle[1]->_parent = pOpposite;
	// If either of the split triangles was in the merge list, remove it.
	static PgDiamond* pDiamond;
	for(pDiamond=_diamondList.GetHead(); pDiamond->IsInList(); pDiamond=pDiamond->GetNext())
	{
		if(pDiamond->hasTriangle(pTriangle) || pDiamond->hasTriangle(pOpposite))
		{
			pDiamond = pDiamond->GetPrevious();
			delete pDiamond->GetNext();
		}
	}
	// Append new diamond created by the split to the planet's merge list
	_diamondList.AddTail(
		new PgDiamond( pTriangle, pOpposite, pNewTriangle[0], pNewTriangle[1] ) );
	return true;
}

void
PgROAMPlanet::merge(PgDiamond* pDiamond)
{
	// Get the vertex at the center of the diamond
	unsigned short nVertexId = pDiamond->_parent[0]->_vertex[1];
	PgVertex* pVertex = pDiamond->_parent[0]->getVertex(1);

	// Need to run this loop twice to merge children back into parents
	for ( int i = 0; i < 2; ++i )
	{
		PgTriangle* pParent = pDiamond->_parent[i]; // parent triangle
		PgTriangle* pChild = pDiamond->_child[i]; // child triangle
		pParent->setVertices( pParent->_vertex[2], pParent->_vertex[0], pChild->_vertex[0] );
		pParent->setEdges( pParent->_edge[2], pChild->_edge[2], pDiamond->_parent[1-i] );
		pChild->_edge[2]->replaceEdge( pChild, pParent );
		delete pChild;
		--_triCount;

		// update priority members:
		if(i == 0)
		{
			PgVector3 v = pParent->getVertex(0)->_vertData.position.midpoint( pParent->getVertex(2)->_vertData.position );
			pParent->_midpoint = v;
			pParent->_offset = (float)pVertex->_vertData.position.magnitude() - v.magnitude(); // store the offset distance 
		}
		else
		{
			pParent->copyPriority(pParent->_edge[2]);
		}

		// Check if the merged parent triangle forms a new diamond structure
		PgTriangle* pTriangles[4];
		pParent->_diamond = 0;
		pTriangles[0] = pParent;
		pTriangles[1] = pTriangles[0]->_edge[0];
		pTriangles[2] = pTriangles[1]->_edge[0];
		pTriangles[3] = pTriangles[2]->_edge[0];
		if ( pTriangles[3]->_edge[0] == pTriangles[0] )
		{
			// Create a diamond with the arguments in the proper order (parents/children)
			PgDiamond* newDiamond = 0;
			if ( pTriangles[1]->_parent == pTriangles[0] && pTriangles[3]->_parent == pTriangles[2] )
				newDiamond = new PgDiamond( pTriangles[0], pTriangles[2], pTriangles[1], pTriangles[3] );
			if ( pTriangles[2]->_parent == pTriangles[1] && pTriangles[0]->_parent == pTriangles[3] )
				newDiamond = new PgDiamond( pTriangles[1], pTriangles[3], pTriangles[2], pTriangles[0] );
			if (newDiamond != 0)
				_diamondList.AddTail( newDiamond);
		}
	}
	pDiamond->_parent[0]->updateVertexNormals();
	pDiamond->_parent[1]->updateVertexNormals();

	delete pDiamond;
	PgVertex::Array.releaseVertex( nVertexId );
}

void
PgROAMPlanet::updateTriangleOffset(PgTriangle* pTriangle)
{
	PgVector3 mid = pTriangle->getVertex(0)->_vertData.position.midpoint( pTriangle->getVertex(2)->_vertData.position );
	pTriangle->_midpoint = mid;
	float elevation;
	unsigned int color;
	_planetStrategy->getSurfaceData(mid, elevation, color);
	pTriangle->_offset = elevation - mid.magnitude();
	pTriangle->_length = pTriangle->getVertex(0)->_vertData.position.distanceSquared( pTriangle->getVertex(2)->_vertData.position );
}

void
PgROAMPlanet::rebuildEdges()
{
	for(PgTriangle* p1 = _triangleList.GetHead(); p1->IsInList(); p1 = p1->GetNext())
	{
		for(PgTriangle* p2 = p1->GetNext(); p2->IsInList(); p2 = p2->GetNext())
		{
			bool match = true;
			if(p2->hasVertex(p1->_vertex[0]) && p2->hasVertex(p1->_vertex[1]))
			{
				p1->_edge[0] = p2;
			}
			else if(p2->hasVertex(p1->_vertex[1]) && p2->hasVertex(p1->_vertex[2]))
			{
				p1->_edge[1] = p2;
			}
			else if(p2->hasVertex(p1->_vertex[2]) && p2->hasVertex(p1->_vertex[0]))
			{
				p1->_edge[2] = p2;
			}
			else
			{
				match = false;
			}
			if( match )
			{
				if(!p1->hasVertex(p2->_vertex[2]))
				{
					p2->_edge[0] = p1;
				}
				else if(!p1->hasVertex(p2->_vertex[0]))
				{
					p2->_edge[1] = p1;
				}
				else
				{
					p2->_edge[2] = p1;
				}
			}
		}
	}
}