#pragma once

#include "include.h"

class Evaluate
{
public:
	Evaluate() {}
	Evaluate(const Evaluate &) {}
	~Evaluate() {}

	virtual Value evaluate(const BoardParser &) const
	{
		return -VALUE_INFINITE;
	}
};
