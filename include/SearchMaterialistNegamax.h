#pragma once

#include "search.h"

#include <algorithm>

class SearchMaterialistNegamax : virtual public Search
{
public:
	SearchMaterialistNegamax(const Search::LimitsType &limits) : Search(limits) {}
	SearchMaterialistNegamax(const SearchMaterialistNegamax &s) : Search(s.Limits) {}
	~SearchMaterialistNegamax() {}

	Int quiesce(const BoardParser &b, const Evaluate &e, Int alpha, Int beta)
	{
		++nodesSearched[pvIdx];
		if (inCheckMate(b, b.isWhiteTurn()))
			return MAX_EVAL; // not sure, could be alpha or beta ?
		// In order to get the quiescence search to terminate, plies are usually restricted to moves that deal directly with the threat,
		// such as moves that capture and recapture (often called a 'capture search') in chess
		Int stand_pat = e.evaluate(b);
		if (stand_pat >= beta)
			return beta;
		if (alpha < stand_pat)
			alpha = stand_pat;


		// Early quit for quiesce
		if (rootMoves[pvIdx].pvDepth >= 7)
			return alpha;

		std::vector<cMove> moveListCaptures = std::vector<cMove>();
		generateMoveList(b, moveListCaptures, /*legalOnly=*/ true, true);

		// Quiet position
		if (moveListCaptures.empty())
		{
			return alpha;
		}

		// Increase depth after early stop
		++(rootMoves[pvIdx].pvDepth);

		BoardParser b2;

		for (const cMove move : moveListCaptures)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);
			Int score = -quiesce(b2, e, -beta, -alpha);

			if (score >= beta)
			{
				--(rootMoves[pvIdx].pvDepth);
				return beta;
			}
			if (score > alpha)
			{
				alpha = score;
			}
		}
		--(rootMoves[pvIdx].pvDepth);
		return alpha;
	}

	// https://www.chessprogramming.org/Alpha-Beta
	Int abSearch(const BoardParser &b, const Evaluate &e, Int alpha, Int beta, UInt depth)
	{
		++nodesSearched[pvIdx];
		if (depth <= 0)
			// return e.evaluate(b);
			return quiesce(b, e, alpha, beta);

		std::vector<cMove> moveList;
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

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);

			Int score = -abSearch(b2, e, -beta, -alpha, depth - 1);
			if (score >= beta)
			{
				// beta cutoff
				return beta;
			}
			if (score > alpha)
			{
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth] = move;
				rootMoveTemp = rootMoves[pvIdx];
				// alpha acts like max in MiniMax
				alpha = score;
			}
			else
			{
				rootMoves[pvIdx] = rootMoveTemp;
			}
		}
		--(rootMoves[pvIdx].pvDepth);
		return alpha;
	}

	// Iterative deepening algorithm
	cMove nextMove(const BoardParser &b, const Evaluate &e) override
	{
		UInt currentDepth = Limits.depth;
		std::vector<cMove> moveList;
		generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		if (moveList.empty())
		{
			err("Cannot move.");
			return cMove();
		}

		rootMovesSize = moveList.size();
		for (UInt i = 0; i < rootMovesSize; ++i)
		{
			rootMoves[i] = RootMove(moveList[i]);
		}

		BoardParser b2;
		Int bestScore = -MAX_EVAL;

		while (currentDepth <= Limits.depth)
		{
			// Some variables have to be reset
			nodesSearched.fill(0);

			for (pvIdx = 0; pvIdx < rootMovesSize; ++pvIdx)
			{
				cMove move = rootMoves[pvIdx].pv[0];
				b2 = BoardParser(b);
				b2.movePiece(move);
				Int score;
				if (currentDepth == 0)
					score = e.evaluate(b2);
				else
				{
					score = -abSearch(b2, e, -MAX_EVAL, +MAX_EVAL, currentDepth - 1);
				}
				rootMoves[pvIdx].score = score;
			}
			std::stable_sort(rootMoves.begin(), rootMoves.end());
			std::cout << UCI::pv(*this, b, currentDepth) << std::endl;

			++currentDepth;
		}
		return rootMoves[0].pv[0];
	}
};
