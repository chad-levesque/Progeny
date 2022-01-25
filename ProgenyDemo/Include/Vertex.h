#ifndef __PROGENY_VERTEX_H__
#define __PROGENY_VERTEX_H__

#include "Color.h"
#include "Vector3.h"

namespace Progeny
{
	class Vertex
	{
	public:
		Vertex(Vector3 position);
		virtual ~Vertex();

		void setX(double x);
		void setY(double y);
		void setZ(double z);
		void setXYZ(double x, double y, double z);
		void setColor(Color color);
		void setU(double u);
		void setV(double v);

		unsigned long getColor() const;
		double getX() const;
		double getY() const;
		double getZ() const;
		double getU() const;
		double getV() const;
		Vector3 getPosition() const;
	
	private:
		Vector3 _position;
		Color   _color;
		double _u;
		double _v;
	};
}

#endif