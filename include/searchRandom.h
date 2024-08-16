#pragma once

#include "search.h"

#include <random>

class SearchRandom : virtual public Search
{
public:
	SearchRandom(const Search::LimitsType &limits, bool *g_stop) : Search(limits, g_stop) {}
	SearchRandom(const SearchRandom &s) : Search(s.Limits, s.g_stop) {}
	~SearchRandom() {}

	cMove nextMove(BoardParser &b, const Evaluate &) override
	{
		const std::lock_guard<std::mutex> lock(mtx);
		// Checking book
		cMove book = probeBook(b);
		if (book != cMove())
			return book;
		std::vector<cMove> moveList;
		Search::generateMoveList(b, moveList, /*legalOnly=*/ true, /*onlyCapture=*/ false);

		if (moveList.empty())
		{
			err("Cannot move after checkmate.");
			return cMove();
		}
		else if (moveList.size() == 1)
		{
			return moveList[0];
		}

		std::random_device rd;
		std::mt19937 gen(rd()); // Mersenne Twister engine
		std::uniform_int_distribution<> dis(0, Int(moveList.size()) - 1);

		UInt idx = dis(gen);
		return moveList[idx];
	}
};
