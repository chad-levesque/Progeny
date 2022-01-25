#include "HorizonPgRenderer.h"

#include <Graphics/Graphics.h>
#include <PgVertex.h>
#include <PgConstants.h>

HorizonPgRenderer::HorizonPgRenderer(Horizon::IGraphics* graphics) :
	_pGraphics( graphics )
{
	//c'tor
	_pVertexType = _pGraphics->CreateVertexType( &VERTEXDESC_PGVERTEX );
}

HorizonPgRenderer::~HorizonPgRenderer()
{
	//d'tor
}

void
HorizonPgRenderer::drawIndexedPrimitives(unsigned short* indexBuffer, Progeny::PgVertex* vertexBuffer, int triangleCount)
{
	// TODO
	_pGraphics->SetVertexType(_pVertexType);
	_pGraphics->SetVertexColorEnable(true);
	_pGraphics->SetTexture(0, NULL);
	while (triangleCount > 0)
	{
		int trianglesThisRender = min(triangleCount, MAX_TRIANGLES_PER_RENDER);
		_pGraphics->RenderIndexedPrimitive(
			indexBuffer,
			false,
			vertexBuffer,
			MAX_NUMBER_VERTICES,
			Horizon::EPT_TRIANGLELIST,
			sizeof(Progeny::PgVertex),
			trianglesThisRender);
		indexBuffer += trianglesThisRender*3;
		triangleCount -= trianglesThisRender;
	}
}
