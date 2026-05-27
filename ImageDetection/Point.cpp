//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "Point.h"

Point::Point()
	: r(0),
	c(0)
{
}

Point::Point(unsigned int _r, unsigned int _c)
	: r(_r),
	c(_c)
{
}

void Point::Print(const char *pName)
{
	Trace::out("%s: (%d,%d)\n", pName, this->c, this->r);
}

// --- End of File ---
