#include "PgTriangle.h"
#include "PgVertex.h"
#include "IPgCamera.h"
#include "MathTools.h"

#include <cstring>

using namespace Progeny;

PgTriangle::PgTriangle(unsigned short n1, unsigned short n2, unsigned short n3)
	: LinkedListNode(),
	 _diamond(NULL),
	 _waitCounter(0)
{
	setVertices(n1, n2, n3);
}

PgTriangle::PgTriangle() :
	LinkedListNode()
{
	memset((char *)this, 0, sizeof(PgTriangle));
}

Progeny::PgVertex*
PgTriangle::getVertex(unsigned short n)
{
	return PgVertex::Array[_vertex[n]];
}

bool
PgTriangle::hasVertex(unsigned short index)
{
	if (_vertex[0] == index || _vertex[1] == index || _vertex[2] == index )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void
PgTriangle::setVertices(unsigned short n1, unsigned short n2, unsigned short n3)
{
	_vertex[0] = n1;
	_vertex[1] = n2;
	_vertex[2] = n3;
	computeNormal();
}

void 
PgTriangle::setEdges( PgTriangle *p1, PgTriangle *p2, PgTriangle *p3 )
{
	_edge[0] = p1;
	_edge[1] = p2;
	_edge[2] = p3;
}

void
PgTriangle::replaceEdge( PgTriangle *current, PgTriangle *replacement )
{
	if( _edge[0] == current )
	{
		_edge[0] = replacement;
	}
	else if( _edge[1] == current )
	{
		_edge[1] = replacement;
	}
	else if( _edge[2] == current )
	{
		_edge[2] = replacement;
	}
}
		
void
PgTriangle::draw(unsigned short *nArray, unsigned int &nIndex)
{
	nArray[nIndex++] = _vertex[0];
	nArray[nIndex++] = _vertex[1];
	nArray[nIndex++] = _vertex[2];
}

float
PgTriangle::getPriority(IPgCamera* pCamera, float horizon)
{
	// Priority = a*Distance + b*Orthogonality + c*Error + d*Cullage
	//PgVector3 cameraPos = pCamera->getCameraPosition( );
	//float priority = 0.0f;
	//PgVector3 viewToMidpoint =_midpoint - cameraPos;
	//if (viewToMidpoint.dot(_midpoint) < 0 && pCamera->isInFrustum((_midpoint)))
	//{
	//	PgVector3 midpointDirection = _midpoint;
	//	midpointDirection.normalize();
	//	PgVector3 errorVec = viewToMidpoint+ midpointDirection*_offset;
	//	viewToMidpoint.normalize();
	//	errorVec.normalize();
	//	priority = abs(viewToMidpoint.dot(errorVec) );
	//	clamp(priority, 0.0f, 1.0f);
	//	// angle between the two vectors, denoting error in screen space as an arcspan:
	//	priority = abs( acosf( priority ) ) ;
	//	clamp(priority, 0.0f, 1.0f);
	//}
	//return priority;
	float fPriority = 0.0f;
	PgVector3 cameraPos = pCamera->getCameraPosition();
	float fDistance = cameraPos.distanceSquared(_midpoint);
	PgVector3 heading = pCamera->getCameraHeading();
	PgVector3 cameraToMidpointDirection = _midpoint-cameraPos;
	cameraToMidpointDirection.normalize();
	// Only calculate priority for vertices closer than the horizon
	// (Make an exception if the triangle is larger than the camera's distance to it)
	if(fDistance < _length || fDistance < horizon )
	{
		// Only calculate priority for vertices inside the angle of view
		// (Make an exception if the triangle is larger than the camera's distance to it)
		if(fDistance < _length || ( heading.dot( cameraToMidpointDirection )) > 0.707f)
		{
			float fError = _offset*_offset;
			if(fDistance < _length && fDistance > 1.0f)
				fError = maxf(fError, _length/fDistance);
			fPriority = fError / sqrtf(fDistance);
		}
	}
	return fPriority;
}

void
PgTriangle::copyPriority(PgTriangle* p)
{
	this->_midpoint = p->_midpoint;
	this->_offset = p->_offset;
	this->_length = p->_length;
}

void 
PgTriangle::computeNormal()
{
	PgVector3 temp1 = getVertex(1)->_vertData.position - getVertex(0)->_vertData.position;
	PgVector3 temp2 = getVertex(2)->_vertData.position - getVertex(0)->_vertData.position;

	_normal = temp1.cross(temp2);
	_normal.normalize();
}

void
PgTriangle::updateVertexNormals()
{
	for (int i = 0; i < 3; ++i)
	{
		PgVertex* vertex = getVertex(i);
		vertex->_vertData.normal = _normal;

		PgTriangle* pPrev = this;
		PgTriangle* pNext = _edge[i];
		while( pNext != this )
		{
			int j = 0;
			vertex->_vertData.normal+=pNext->_normal;
			for ( ; j < 3; ++j )
			{
				if(pNext->_edge[j] != pPrev && pNext->_edge[j]->hasVertex(_vertex[i]))
				{
					pPrev = pNext;
					pNext = pNext->_edge[j];
					break;
				}

			}
			if (j == 3)
			{
				break;
			}
		}
		vertex->_vertData.normal.normalize();
		// TODO: If we ever switched to buffered rendering, set updated flag here
	}
}