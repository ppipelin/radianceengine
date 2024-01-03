#pragma once

#include "search.h"

#include <algorithm>

class SearchMaterialist : virtual public Search
{
public:
	SearchMaterialist(const Search::LimitsType &limits) : Search(limits) {}
	SearchMaterialist(const SearchMaterialist &s) : Search(s.Limits) {}
	~SearchMaterialist() {}

	Int search(const BoardParser &b, const Evaluate &e, UInt depth)
	{
		++nodesSearched[pvIdx];
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

		// Increase depth after early stop
		++(rootMoves[pvIdx].pvDepth);

		BoardParser b2;
		Int bestScore = -MAX_EVAL;

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score = -search(b2, e, depth - 1);
			if (score > bestScore)
			{
				bestScore = score;
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth] = move;
				rootMoveTemp = rootMoves[pvIdx];
				if (bestScore == MAX_EVAL)
					break;
			}
			else
			{
				// rollback
				rootMoves[pvIdx] = rootMoveTemp;
			}
		}
		--(rootMoves[pvIdx].pvDepth);
		return bestScore;
	}

	cMove nextMove(const BoardParser &b, const Evaluate &e) override
	{
		nodesSearched.fill(0);
		std::vector<cMove> moveList = std::vector<cMove>();
		generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			return cMove();
		}

		rootMovesSize = moveList.size();
		for (UInt i = 0; i < rootMovesSize; ++i)
		{
			rootMoves[i] = RootMove(moveList[i]);
		}

		BoardParser b2;
		cMove bestMove;

		for (pvIdx = 0; pvIdx < rootMovesSize; ++pvIdx)
		{
			cMove move = rootMoves[pvIdx].pv[0];
			if (move.isCapture() && move.getTo() == 11) //  && (*b.boardParsed())[60] != nullptr
			{
				std::cout << "";
			}
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score;
			if (Limits.depth == 0)
				score = e.evaluate(b2);
			else
				score = -search(b2, e, Limits.depth - 1);
			rootMoves[pvIdx].score = score;
		}
		std::stable_sort(rootMoves.begin(), rootMoves.end());
		std::cout << UCI::pv(*this, b, Limits.depth) << std::endl;
		return rootMoves[0].pv[0];
	}
};
