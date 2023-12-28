#pragma once

#include "include.h"

class Evaluate
{
public:
	Evaluate() {}
	Evaluate(const Evaluate &) {}
	~Evaluate() {}

	virtual Int evaluate(const BoardParser &) const
	{
		return -MAX_EVAL;
	}
};
