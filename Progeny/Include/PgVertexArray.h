#ifndef __PROGENY_VERTEX_ARRAY_H__
#define __PROGENY_VERTEX_ARRAY_H__

namespace Progeny
{
	class PgVertex;

	class PgVertexArray
	{
	public:
		PgVertexArray( int arraySize, unsigned short elementSize );
		PgVertex* operator[](unsigned short n)	{ return (PgVertex*)((int)_buffer + (int)(_vertexSize * n)); } // [] override
		void init();
		void reset();
		bool acquireVertex( unsigned short& index );
		void releaseVertex( unsigned short index );
		int getVertexCount();
	private:
		void* _buffer;
		int _arraySize;
		int _vertexSize;
		int _usedVertices;
		unsigned short _nextFreeVertex;
		unsigned short* _freeVertexStack;
	};
}

#endif //__PROGENY_VERTEX_ARRAY_H__