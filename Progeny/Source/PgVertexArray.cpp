#include "PgVertexArray.h"

#include <cstring>

Progeny::PgVertexArray::PgVertexArray( int arraySize, unsigned short vertexSize ) :
	_arraySize( arraySize ),
	_vertexSize( vertexSize ),
	_usedVertices( 0 )
{
	init();
}

void
Progeny::PgVertexArray::init()
{
	_nextFreeVertex = 0;
	_freeVertexStack = new unsigned short[_arraySize];
	_buffer = new char[_arraySize*_vertexSize];
	memset(_buffer, 0, _arraySize * _vertexSize);
	for (int i = 0; i < _arraySize; ++i) 
	{
		_freeVertexStack[i] = i;
	}
}

void
Progeny::PgVertexArray::reset()
{
	_usedVertices = 0;
	_nextFreeVertex = 0;
	memset(_buffer, 0, _arraySize * _vertexSize);
	for (int i = 0; i < _arraySize; ++i) 
	{
		_freeVertexStack[i] = i;
	}
}

bool
Progeny::PgVertexArray::acquireVertex( unsigned short& index )
{
	// TODO array expanding by block size
	if ( _nextFreeVertex < _arraySize)
	{
		index = _freeVertexStack[_nextFreeVertex++];
		++_usedVertices;
		return true;
	}
	else
	{
		return false;
	}
}

void
Progeny::PgVertexArray::releaseVertex( unsigned short index )
{
	_freeVertexStack[--_nextFreeVertex] = index;
	--_usedVertices;
}

int
Progeny::PgVertexArray::getVertexCount()
{
	return _usedVertices;
}