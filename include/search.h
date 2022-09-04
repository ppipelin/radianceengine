#pragma once

#include "include.h"
#include "cMove.h"

class Search
{
public:
	Search() {}
	Search(const Search &) {}
	~Search() {}
	virtual cMove nextMove(BoardParser *)
	{
		return cMove(0, 0);
	}
};
