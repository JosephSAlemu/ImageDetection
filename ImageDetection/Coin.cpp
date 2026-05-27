//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "Coin.h"

Coin::Coin()
	: value(0.0f),
	diameter(0.0f),
	type(Type::unknown),
	pt()
{
}

void Coin::Print()
{
	if(type == Type::Dime)
	{
		Trace::out("\t     Type: Dime\n");
	}
	else if(type == Type::Penny)
	{
		Trace::out("\t     Type: Penny\n");
	}
	else if(type == Type::Nickel)
	{
		Trace::out("\t     Type: Nickel\n");
	}
	else
	{
		Trace::out("\t     Type: Quarter\n");
	}

	Trace::out("\t(row,col): %d %d \n", pt.r, pt.c);
	Trace::out("\t    value: %f \n", value);
	Trace::out("\t diameter: %f \n", diameter);
	Trace::out("\n");
}

void Coin::Set(Point &_pt, float _diameter)
{
	this->pt.r = _pt.r;
	this->pt.c = _pt.c;
	this->diameter = _diameter;

	if(this->diameter < 334.0f)
	{
		this->type = Type::Dime;
		this->value = 0.10f;
	}
	else if(this->diameter < 360.0f)
	{
		this->type = Type::Penny;
		this->value = 0.01f;
	}

	else if(this->diameter < 400.0f)
	{
		this->type = Type::Nickel;
		this->value = 0.05f;
	}
	else
	{
		this->type = Type::Quarter;
		this->value = 0.25f;
	}
}


// --- End of File ---
