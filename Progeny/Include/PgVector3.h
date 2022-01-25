#ifndef	__PROGENY_VECTOR3_H__
#define __PROGENY_VECTOR3_H__

#include <math.h>

namespace Progeny
{
	class PgVector3
	{
	public:
		PgVector3() { PgVector3(0.0, 0.0, 0.0); }
		PgVector3(float x, float y, float z) : 
		_x( x), _y(y), _z(z) { }
		float _x;
		float _y;
		float _z;

		PgVector3 midpoint( PgVector3 &vert) const { return PgVector3( (*this - vert) * 0.5f + vert ) ;}
		float magnitudeSquared() const { return _x*_x + _y*_y + _z*_z; }
		float magnitude() const { return sqrtf(magnitudeSquared()); }
		void normalize() { return *this/= magnitude(); }
		float distanceSquared(PgVector3& v) const { return (*this-v).magnitudeSquared(); }
		float distance(PgVector3& v) const { return sqrt(distanceSquared(v) ); }
		float dot(PgVector3& v) const { return _x*v._x + _y*v._y + _z*v._z; }

		// Vector Operations
		PgVector3 cross ( PgVector3 &vert ) const { return PgVector3(_y*vert._z - _z*vert._y, _z*vert._x - _x*vert._z, _x*vert._y - _y*vert._x); }

		PgVector3 operator+(PgVector3 &v) const { return PgVector3( _x+v._x, _y+v._y, _z+v._z) ;}
		PgVector3 operator-(PgVector3 &v) const { return PgVector3( _x-v._x, _y-v._y, _z-v._z) ;}
		PgVector3 operator*(PgVector3 &v) const { return PgVector3( _x*v._x, _y*v._y, _z*v._z );}
		void operator+=(PgVector3 &v) { _x+=v._x; _y+=v._y; _z+=v._z; }
		
		PgVector3 operator*(float f) const { return PgVector3( _x*f, _y*f, _z*f); }
		void operator*=(float f) { _x*=f; _y*=f; _z*=f; }
		void operator/=(float f) {  f=1/f; _x*=f; _y*=f; _z*=f; }
	};
}


#endif