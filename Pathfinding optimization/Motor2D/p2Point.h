// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __P2POINT_H__
#define __P2POINT_H__

#include "p2Defs.h"
#include <math.h>

template<class TYPE>
class p2Point
{
public:

	TYPE x, y;

	p2Point()
	{}

	p2Point(const p2Point<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	p2Point(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	p2Point& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	// Math ------------------------------------------------
	p2Point operator -(const p2Point &v) const
	{
		p2Vector2 r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	p2Point operator + (const p2Point &v) const
	{
		p2Vector2 r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const p2Point& operator -=(const p2Point &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const p2Point& operator +=(const p2Point &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const p2Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const p2Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	p2Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	p2Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const p2Point& v) const
	{
		int dx = abs(v.x - x);
		int dy = abs(v.y - y);
		if (dx > dy) return (14 * dy + 10 * (dx - dy));
		else return 14 * dx + 10 *(dy - dx);
	}
	TYPE DistanceToh(const p2Point& v) const
	{
		int dx = abs(v.x - x);
		int dy = abs(v.y - y);
		if (dx > dy) return (dy + (dx - dy));
		else return dx + (dy - dx);
	}

	TYPE DistanceNoSqrt(const p2Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE DistanceHeuristic(const p2Point& v) const
	{
		int dx = abs(v.x - x);
		int dy = abs(v.y - y);
		int res =(dx + dy);
		return res;
	}
};

typedef p2Point<int> iPoint;
typedef p2Point<float> fPoint;

#endif // __P2POINT_H__