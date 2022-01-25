#include "Vertex.h"
#include "HexColor.h"

using namespace Progeny;


Vertex::Vertex(Vector3 position) :
	_position(position),
	_color(HexColor::White())
{
}

Vertex::~Vertex()
{
}

void Vertex::setX( double x )
{
	_position.setX(x);
}

void Vertex::setY( double y )
{
	_position.setY(y);
}

void Vertex::setZ( double z )
{
	_position.setZ(z);
}

void Vertex::setXYZ( double x, double y, double z )
{
	_position.setXYZ(x, y, z);
}

void Vertex::setColor(Color color)
{	
	_color.setHexColor(color.getHexValue());
}

void Vertex::setU(double u) {
	_u = u;
}

void Vertex::setV(double v) {
	_v = v;
}
unsigned long Vertex::getColor() const
{
	HexColor color = _color.getHexValue();
	return color.getValue();
}

double Vertex::getX() const
{
	return _position.getX();
}

double Vertex::getY() const
{
	return _position.getY();
}

double Vertex::getZ() const
{
	return _position.getZ();
}

double Vertex::getU() const
{
	return _u;
}

double Vertex::getV() const
{
	return _v;
}

Vector3 Vertex::getPosition() const {
	return _position;
}