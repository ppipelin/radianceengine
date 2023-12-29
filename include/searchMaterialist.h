#pragma once

#include "search.h"

class SearchMaterialist : virtual public Search
{
public:
	SearchMaterialist(UInt maxDepth = 1) : Search(maxDepth) {}
	SearchMaterialist(const SearchMaterialist &) {}
	~SearchMaterialist() {}

	// Int abSearch(const BoardParser &b, const Evaluate &e, Int alpha, Int beta, UInt depth) const
	std::pair<Int, cMove> search(const BoardParser &b, const Evaluate &e, UInt depth) const
	{
		if (depth <= 0)
			return std::pair<Int, cMove>(e.evaluate(b), cMove());

		std::vector<cMove> moveList = std::vector<cMove>();
		generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			if (b.inCheck(b.isWhiteTurn()))
				return std::pair<Int, cMove>(-MAX_EVAL, cMove());
			return std::pair<Int, cMove>(0, cMove());
		}

		BoardParser b2;
		Int bestScore = -MAX_EVAL;
		cMove bestMove;

		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score = -search(b2, e, depth - 1).first;
			if (score > bestScore)
			{
				bestScore = score;
				bestMove = move;
				if (bestScore == MAX_EVAL)
					break;
			}
		}
		return std::pair<Int, cMove>(bestScore, bestMove);
	}

	cMove nextMove(const BoardParser &b, const Evaluate &e) const override
	{
		std::pair<Int, cMove> pair = search(b, e, m_maxDepth);
		return pair.second;
	}
};
