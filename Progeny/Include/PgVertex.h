#ifndef __PROGENY_VERTEX_H__
#define __PROGENY_VERTEX_H__

#include "PgVector3.h"
#include "PgVertexArray.h"

namespace Progeny
{
	struct VertexData
	{
		PgVector3 position;
		PgVector3 normal;
		float textureCoordinates[2];
		unsigned int color;
	};

	class PgVertex
	{
	public:
		static PgVertexArray Array;
		VertexData _vertData;
	public:
		PgVertex()	{ }
		void Init(PgVector3 &v)
		{
			_vertData.position = v;
			_vertData.normal = v;
			_vertData.textureCoordinates[0] = _vertData.textureCoordinates[1]  = 0.0f;
			_vertData.color = 0xFFFFFFFF;
		}
	};
}

#endif