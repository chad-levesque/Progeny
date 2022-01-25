#ifndef __PROGENYHORIZONPGRENDERER_H__
#define __PROGENYHORIZONPGRENDERER_H__

#include "IPgRenderer.h"
#include <Graphics/GraphicsTypes.h>
#include <Math/MathTypes.h>

#define MAX_TRIANGLES_PER_RENDER 10000


namespace Horizon {
	class IGraphics;
	class IVertexBuffer;
	class IIndexBuffer;
	class IVertexType;
}

namespace Progeny {
	class PgVertex;
}

const Horizon::VERTEXDESC VERTEXDESC_PGVERTEX =
{
	4,
	{
		{0, 0, Horizon::TYPE_FLOAT3, Horizon::USAGE_POSITION, 0},
		{0, 12, Horizon::TYPE_FLOAT3, Horizon::USAGE_NORMAL, 0},
		{0, 24, Horizon::TYPE_FLOAT2, Horizon::USAGE_TEXCOORD, 0},
		{0, 32, Horizon::TYPE_COLOR, Horizon::USAGE_COLOR, 0}
	}
};

class HorizonPgRenderer : public Progeny::IPgRenderer
{
public:
	HorizonPgRenderer(Horizon::IGraphics* graphics);
	virtual ~HorizonPgRenderer();
	virtual void drawIndexedPrimitives(unsigned short* indexBuffer, Progeny::PgVertex* vertexBuffer, int triangleCount);
private:
	Horizon::IGraphics* _pGraphics;
	Horizon::IVertexBuffer* _pVertexBuffer;
	Horizon::IIndexBuffer* _pIndexBuffer;
	Horizon::IVertexType* _pVertexType;
};

#endif