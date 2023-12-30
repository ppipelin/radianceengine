#pragma once

#include "search.h"

class SearchMaterialist : virtual public Search
{
public:
	SearchMaterialist(const Search::LimitsType &limits) : Search(limits) {}
	SearchMaterialist(const SearchMaterialist &s) : Search(s.Limits) {}
	~SearchMaterialist() {}

	// Int abSearch(const BoardParser &b, const Evaluate &e, Int alpha, Int beta, UInt depth) const
	Int search(const BoardParser &b, const Evaluate &e, UInt depth) const
	{
		if (depth <= 0)
			return e.evaluate(b);

		std::vector<cMove> moveList = std::vector<cMove>();
		generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			if (b.inCheck(b.isWhiteTurn()))
				return -MAX_EVAL;
			return 0;
		}

		BoardParser b2;
		Int bestScore = -MAX_EVAL;

		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score = -search(b2, e, depth - 1);
			if (score > bestScore)
			{
				bestScore = score;
				if (bestScore == MAX_EVAL)
					break;
			}
		}
		return bestScore;
	}

	cMove nextMove(const BoardParser &b, const Evaluate &e) override
	{
		std::vector<cMove> moveList = std::vector<cMove>();
		generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			return cMove();
		}

		BoardParser b2;
		Int bestScore = -MAX_EVAL;
		cMove bestMove;

		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score;
			if (Limits.depth == 0)
				score = e.evaluate(b2);
			else
				score = -search(b2, e, Limits.depth - 1);

			if (score > bestScore)
			{
				bestScore = score;
				bestMove = move;
				if (bestScore == MAX_EVAL)
					break;
			}
		}
		return bestMove;
	}
};
