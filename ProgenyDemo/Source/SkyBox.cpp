#include "SkyBox.h"

#include "CameraSystem.h"
#include "DemoConstants.h"


SkyBox::SkyBox() :
	_height(1.0),
	_length(1.0),
	_width(1.0),
	_skyOpacity(0.0f),
	_skyColor(DEFAULT_SKY_COLOR)
{
	generateMesh();
}

SkyBox::SkyBox( double length, double width, double height ) :
	_height(height),
	_length(length),
	_width(width)
{
	generateMesh();
	generateSun();
}

SkyBox::~SkyBox()
{
	
}

void
SkyBox::setSkyOpacity(float opacity)
{
	_skyOpacity = opacity;
}

void
SkyBox::setSkyColor(unsigned int color)
{
	_skyColor = color;
}

void SkyBox::generateMesh()
{
	double length = 0.5*_length;
	double width  = 0.5*_width;
	double height = 0.5*_height;
	_vertCount = 17;	
	_vertices = new Horizon::VERTEX_XYZTEX[_vertCount];
	addVertex(0, -length, -height, -width);
	addVertex(1, -length, -height,  width);
	addVertex(2, -length,  height, -width);
	addVertex(3, -length,  height,  width);
	addVertex(4,  length, -height, -width);
	addVertex(5,  length, -height,  width);
	addVertex(6,  length,  height, -width);
	addVertex(7,  length,  height,  width);
	addVertex(8, -length,  height, -width);
	addVertex(9, -length, -height,  width);
	addVertex(10, length, -height,  width);
	addVertex(11,-length,  height, -width);
	addVertex(12, length,  height, -width);
	addVertex(13,-length, -height, -width);
	addVertex(14, length, -height, -width);
	addVertex(15,-length,  height,  width);
	addVertex(16, length,  height,  width);

	setUV(0,  0.0f,      1.0f);
	setUV(1,  1.0f/6.0f, 1.0f);
	setUV(2,  0.0f,      0.0f);
	setUV(3,  1.0f/6.0f, 0.0f);
	setUV(4,  3.0f/6.0f, 1.0f);
	setUV(5,  2.0f/6.0f, 1.0f);
	setUV(6,  3.0f/6.0f, 0.0f);
	setUV(7,  2.0f/6.0f, 0.0f);
	setUV(8,  4.0f/6.0f, 0.0f);
	setUV(9,  4.0f/6.0f, 0.0f);
	setUV(10, 5.0f/6.0f, 0.0f);
	setUV(11, 5.0f/6.0f, 0.0f);
	setUV(12, 1.0f,      0.0f);
	setUV(13, 4.0f/6.0f, 1.0f);
	setUV(14, 5.0f/6.0f, 1.0f);
	setUV(15, 5.0f/6.0f, 1.0f);
	setUV(16, 1.0f,      1.0f);

	_triangles =  new MyTriangle[12];

	addTriangle(0, 15, 16, 12);
	addTriangle(1, 11, 15 , 12);
	addTriangle(2, 13, 14, 10);
	addTriangle(3, 9, 13, 10);
	addTriangle(4, 4, 13, 8);
	addTriangle(5, 6, 4, 8);
	addTriangle(6, 5, 4, 6);
	addTriangle(7, 7, 5, 6);
	addTriangle(8, 1, 5, 7);
	addTriangle(9, 3, 1, 7);
	addTriangle(10, 0, 1 , 3);
	addTriangle(11, 2, 0, 3);

	
	Horizon::IGraphics* pGraphics = Horizon::IGraphics::GetInstance();
	_vertexType = pGraphics->CreateVertexType(&Horizon::VERTEXDESC_XYZTEX);
	_pSkyboxTexture = pGraphics->CreateTextureFromFile("Data/Images/skybox/skybox_2_dark.tga");
}

void
SkyBox::generateSun()
{
	_sunVertices = new Horizon::VERTEX_XYZTEX[4];
	float halfSun = SUN_SIZE/2.0f;

	_sunVertices[0].v = Horizon::VECTOR3(-halfSun, halfSun, 0.0f);
	_sunVertices[0].t.x = 0.0f; _sunVertices[0].t.y = 0.0f;
	_sunVertices[0].color = 0xFFFFFFFF;

	_sunVertices[1].v = Horizon::VECTOR3(-halfSun, -halfSun, 0.0f);
	_sunVertices[1].t.x = 0.0f; _sunVertices[1].t.y = 1.0f;
	_sunVertices[1].color = 0xFFFFFFFF;

	_sunVertices[2].v = Horizon::VECTOR3(halfSun, -halfSun, 0.0f);
	_sunVertices[2].t.x = 1.0f; _sunVertices[2].t.y = 1.0f;
	_sunVertices[2].color = 0xFFFFFFFF;

	_sunVertices[3].v = Horizon::VECTOR3(halfSun, halfSun, 0.0f);
	_sunVertices[3].t.x = 1.0f; _sunVertices[3].t.y = 0.0f;
	_sunVertices[3].color = 0xFFFFFFFF;

	_sunTriangles = new MyTriangle[2];
	_sunTriangles[0].point1 = _sunVertices[0];
	_sunTriangles[0].point2 = _sunVertices[1];
	_sunTriangles[0].point3 = _sunVertices[2];
	_sunTriangles[1].point1 = _sunVertices[2];
	_sunTriangles[1].point2 = _sunVertices[3];
	_sunTriangles[1].point3 = _sunVertices[0];

	Horizon::IGraphics* pGraphics = Horizon::IGraphics::GetInstance();
	_pSunTexture = pGraphics->GetTextureManager()->CreateTextureFromFile("Data/Images/sun.png");
}

void
SkyBox::render(Horizon::IGraphics* pGraphics, CameraSystem* pCamera, Horizon::VECTOR3& sunPosition)
{
	pGraphics->SetTextureFactor(0xFFFFFFFF);
	pGraphics->SetTextureColorArg0(0, Horizon::TEXTURESOURCE_TEXTURE_COLOR);
	pGraphics->SetTextureColorArg1(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	pGraphics->SetTextureColorOp(0, Horizon::TEXTUREOP_MODULATE);
	pGraphics->SetTextureAlphaArg0(0, Horizon::TEXTURESOURCE_TEXTURE_ALPHA);
	pGraphics->SetTextureAlphaArg1(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	pGraphics->SetTextureAlphaOp(0, Horizon::TEXTUREOP_MODULATE);
	// render skybox
	pGraphics->SetDepthEnable(false);
	pGraphics->SetDepthWriteEnable(false);
	pGraphics->SetTexture(0, _pSkyboxTexture);
	Progeny::PgVector3 cameraPos = pCamera->getCameraPosition();
	pGraphics->PushMatrix();
	pGraphics->Translate(cameraPos._x, cameraPos._y, cameraPos._z);
	pGraphics->SetVertexType(_vertexType);
	pGraphics->RenderPrimitives(_triangles, Horizon::EPT_TRIANGLELIST, sizeof(Horizon::VERTEX_XYZTEX), 12);
	pGraphics->PopMatrix();

	unsigned int skyColorAlpha = (0xFF000000 & _skyColor) >> 24;

	unsigned int x = ((unsigned int)(_skyOpacity * skyColorAlpha)) << 24;
	x = x | (_skyColor & 0x00FFFFFF);
	pGraphics->SetTextureFactor(x);
	pGraphics->SetTextureColorArg0(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	pGraphics->SetTextureAlphaArg0(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	Horizon::MATRIX44 scale;
	Horizon::MatrixScale(&scale, 0.8f);
	pGraphics->PushMatrix();
	pGraphics->ApplyMatrix(&scale);
	pGraphics->SetTexture(0, NULL);
	pGraphics->Translate(cameraPos._x, cameraPos._y, cameraPos._z);
	pGraphics->SetVertexType(_vertexType);
	pGraphics->RenderPrimitives(_triangles, Horizon::EPT_TRIANGLELIST, sizeof(Horizon::VERTEX_XYZTEX), 12);
	pGraphics->PopMatrix();

	// render sun
	pGraphics->SetTextureFactor(0xFFFFFFFF);
	pGraphics->SetDepthEnable(true);
	pGraphics->SetDepthWriteEnable(false);
	pGraphics->SetTexture(0, _pSunTexture);
	pGraphics->SetAlphaBlendEnable(true);
	pGraphics->SetAlphaTestEnable(true);
	pGraphics->SetAlphaFunction(Horizon::CMPFUNC_GREATEREQUAL);
	pGraphics->SetAlphaReference((UINT8)8);
	pGraphics->SetTextureColorArg0(0, Horizon::TEXTURESOURCE_TEXTURE_COLOR);
	pGraphics->SetTextureColorArg1(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	pGraphics->SetTextureColorOp(0, Horizon::TEXTUREOP_MODULATE);
	pGraphics->SetTextureAlphaArg0(0, Horizon::TEXTURESOURCE_TEXTURE_ALPHA);
	pGraphics->SetTextureAlphaArg1(0, Horizon::TEXTURESOURCE_TEXTURE_FACTOR);
	pGraphics->SetTextureAlphaOp(0, Horizon::TEXTUREOP_MODULATE);

	Horizon::MATRIX44* viewTransform = pGraphics->GetCamera()->GetViewTransform();
	Horizon::MATRIX44 inverseRotation;
	Horizon::MatrixInvert(viewTransform, &inverseRotation);
	inverseRotation.m14 = sunPosition.x;
	inverseRotation.m24 = sunPosition.y;
	inverseRotation.m34 = sunPosition.z;
	pGraphics->PushMatrix();
	pGraphics->ApplyMatrix(&inverseRotation);
	pGraphics->RenderPrimitives(_sunTriangles, Horizon::EPT_TRIANGLELIST, sizeof(Horizon::VERTEX_XYZTEX), 2);
	pGraphics->PopMatrix();
	pGraphics->SetTexture(0, NULL);
}

void
SkyBox::addVertex(int i, double x, double y, double z)
{
	Horizon::VECTOR3 vert(x, y, z);
	_vertices[i].v = vert;
	_vertices[i].color = 0xFFFFFFFF;
}

void
SkyBox::setUV(int i, double u, double v)
{
	_vertices[i].t.x = u;
	_vertices[i].t.y = v;
}

void
SkyBox::addTriangle(int triIndex, int vertIndex0, int vertIndex1, int vertIndex2)
{
	MyTriangle triangle;
	_triangles[triIndex] = triangle;
	_triangles[triIndex].point1 = _vertices[vertIndex0];
	_triangles[triIndex].point2 = _vertices[vertIndex1];
	_triangles[triIndex].point3 = _vertices[vertIndex2];
}
