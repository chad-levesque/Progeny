#include "PgDiamond.h"

#include <cstring>
#include "PgTriangle.h"
#include "PgVector3.h"
#include "IPgCamera.h"
#include "MathTools.h"

using namespace Progeny;

PgDiamond::PgDiamond(PgTriangle* parent1, PgTriangle* parent2, PgTriangle* child1, PgTriangle* child2 ) :
	LinkedListNode(),
	_waitCounter(0)
{
	_parent[0] = parent1;
	_parent[1] = parent2;
	
	_child[0] = child1;
	_child[1] = child2;

	// Calculate the midpoint of this diamond structure
	_midpoint = parent1->getVertex(1)->_vertData.position;
	_origMidpoint = _parent[0]->getVertex(2)->_vertData.position.midpoint( _child[0]->getVertex(0)->_vertData.position );
	// Calculate the offset of the diamond middle
	_offset = _midpoint.distance( _origMidpoint );

	// Calculate the length^2 of the diamond diagonal
	_length = _parent[0]->getVertex(2)->_vertData.position.distanceSquared(_child[0]->getVertex(0)->_vertData.position);
	// Attach this diamond to the incoming triangles
	_parent[0]->_diamond = this;
	_parent[1]->_diamond = this;
	_child[0]->_diamond = this;
	_child[1]->_diamond = this;
}

float
PgDiamond::getPriority( IPgCamera* pCamera, float horizon )
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
	//	priority = 2.0f*abs( acosf( priority ) ) ;
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
			fPriority = 1.01f * fError / sqrtf(fDistance);
		}
	}
	return fPriority;
}

bool
PgDiamond::hasTriangle(PgTriangle* triangle)
{
	return _parent[0] == triangle || _parent[1] == triangle || _child[0] == triangle || _child[1] == triangle;
}