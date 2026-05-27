//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef POINT_H
#define POINT_H

struct Point
{
	Point();
	Point(const Point &) = default;
	Point &operator = (const Point &) = default;
	~Point() = default;

	Point(unsigned int _r, unsigned int _c);

	void Print(const char *pName);

	unsigned int r;
	unsigned int c;
};

#endif

// --- End of File ---
