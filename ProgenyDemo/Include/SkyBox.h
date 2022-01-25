#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "Graphics/Graphics.h"

class CameraSystem;

class SkyBox
{
public:
	SkyBox( );
	SkyBox( double l, double w, double h );
	~SkyBox();
	struct MyTriangle
	{
		Horizon::VERTEX_XYZTEX point1;
		Horizon::VERTEX_XYZTEX point2;
		Horizon::VERTEX_XYZTEX point3;
	};

	void render(Horizon::IGraphics* pGraphics, CameraSystem* pCamera, Horizon::VECTOR3& sunPosition);
	void setSkyOpacity(float opacity); // between 0 - 1.0
	void setSkyColor(unsigned int color); // 0xAARRGGBB
private:
	virtual void generateMesh();
	virtual void generateSun();
	virtual void addVertex(int index, double x, double y, double z);
	virtual void setUV(int vertIndex, double u, double v);
	virtual void addTriangle(int triIndex, int vertIndex0, int vertIndex1, int vertIndex2);
	Horizon::VERTEX_XYZTEX* _vertices;
	MyTriangle* _triangles;
	double _width;
	double _length;
	double _height;
	int _vertCount;
	Horizon::ITexture* _pSkyboxTexture;
	Horizon::IVertexType* _vertexType;
	Horizon::VERTEX_XYZTEX* _sunVertices;
	Horizon::ITexture* _pSunTexture;
	MyTriangle* _sunTriangles;
	float _skyOpacity;
	unsigned int _skyColor;
};

#endif //__SKYBOX_H__