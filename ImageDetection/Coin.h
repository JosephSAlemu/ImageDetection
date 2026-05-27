//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef COIN_H
#define COIN_H

#include "Point.h"

struct Coin
{
	enum class Type
	{
		Dime,
		Penny,
		Nickel,
		Quarter,
		unknown
	};

	Coin();
	Coin(const Coin &r) = default;
	Coin &operator = (const Coin &r) = default;
	~Coin() = default;

	void Set(Point &pt, float diameter);
	void Print();

	float value;
	float diameter;
	Type  type;
	Point pt;
};

#endif

// --- End of File ---
