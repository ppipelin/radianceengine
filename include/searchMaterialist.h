#pragma once

#include "search.h"

#include <algorithm>

class SearchMaterialist : virtual public Search
{
public:
	SearchMaterialist(const Search::LimitsType &limits, bool *g_stop) : Search(limits, g_stop) {}
	SearchMaterialist(const SearchMaterialist &s) : Search(s.Limits, s.g_stop) {}
	~SearchMaterialist() {}

	Value search(BoardParser &b, const Evaluate &e, UInt depth)
	{
		++nodesSearched[pvIdx];
		if (depth <= 0)
			return e.evaluate(b);

		std::vector<cMove> moveList;
		Search::generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			if (b.inCheck(b.isWhiteTurn()))
				return -VALUE_INFINITE;
			return 0;
		}

		// Increase depth after early stop
		++(rootMoves[pvIdx].pvDepth);

		BoardParser::State s;
		BoardParser b2;
		Value bestScore = -VALUE_INFINITE;

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);

			b2.movePiece(move, s);
			Value score = -search(b2, e, depth - 1);
			if (score > bestScore)
			{
				bestScore = score;
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth] = move;
				rootMoveTemp = rootMoves[pvIdx];
				if (bestScore == VALUE_MATE)
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

	cMove nextMove(BoardParser &b, const Evaluate &e) override
	{
		const std::lock_guard<std::mutex> lock(mtx);
		nodesSearched.fill(0);
		std::vector<cMove> moveList;
		Search::generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			return cMove();
		}

		rootMovesSize = UInt(moveList.size());
		for (UInt i = 0; i < rootMovesSize; ++i)
		{
			rootMoves[i] = RootMove(moveList[i]);
		}

		BoardParser b2;
		cMove bestMove;

		for (pvIdx = 0; pvIdx < rootMovesSize; ++pvIdx)
		{
			cMove move = rootMoves[pvIdx].pv[0];
			b2 = BoardParser(b);
			BoardParser::State s(b2);
			b2.movePiece(move, s);
			Value score;
			if (Limits.depth == 0)
				score = e.evaluate(b2);
			else
				score = -search(b2, e, Limits.depth - 1);
			rootMoves[pvIdx].score = score;
		}
		std::stable_sort(rootMoves.begin(), rootMoves.end());
		std::cout << UCI::pv(*this, Limits.depth) << std::endl;
		return rootMoves[0].pv[0];
	}
};
