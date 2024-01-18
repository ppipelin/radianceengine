#pragma once

#include "search.h"

#include <algorithm>
#include <vector>
#include <fstream>
#include <numeric>

class SearchMaterialistNegamax : virtual public Search
{
	enum NodeType
	{
		NonPV,
		PV,
		Root
	};
public:
	SearchMaterialistNegamax(const Search::LimitsType &limits) : Search(limits) {}
	SearchMaterialistNegamax(const SearchMaterialistNegamax &s) : Search(s.Limits) {}
	~SearchMaterialistNegamax() {}

	Int quiesce(const BoardParser &b, const Evaluate &e, Int alpha, Int beta)
	{
		++nodesSearched[pvIdx];
		if (Search::inCheckMate(b, b.isWhiteTurn()))
			return -MAX_EVAL + rootMoves[pvIdx].pvDepth;
		// In order to get the quiescence search to terminate, plies are usually restricted to moves that deal directly with the threat,
		// such as moves that capture and recapture (often called a 'capture search') in chess
		Int stand_pat = e.evaluate(b);
		if (stand_pat >= beta)
			return beta;
		if (alpha < stand_pat)
			alpha = stand_pat;

		// Early quit for quiesce
		if (rootMoves[pvIdx].pvDepth >= 6)
			return alpha;

		std::vector<cMove> moveListCaptures = std::vector<cMove>();
		Search::generateMoveList(b, moveListCaptures, /*legalOnly=*/ true, true);

		Search::orderMoves(b, moveListCaptures);

		// Quiet position
		if (moveListCaptures.empty())
		{
			return alpha;
		}

		++(rootMoves[pvIdx].pvDepth);

		BoardParser b2;

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveListCaptures)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);

			Int score = -quiesce(b2, e, -beta, -alpha);

			if (score >= beta)
			{
				// beta cutoff
				--(rootMoves[pvIdx].pvDepth);
				return beta;
			}
			if (score > alpha)
			{
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth - 1] = move;
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

	// https://www.chessprogramming.org/Alpha-Beta
	template <NodeType nodeType>
	Int abSearch(const BoardParser &b, const Evaluate &e, Int alpha, Int beta, UInt depth)
	{
		++nodesSearched[pvIdx];

		constexpr bool PvNode = nodeType != NonPV;
		constexpr bool rootNode = nodeType == Root;

		if (depth <= 0)
		{
			// return e.evaluate(b);
			return quiesce(b, e, alpha, beta);
		}
		std::vector<cMove> moveList;
		Search::generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		Search::orderMoves(b, moveList);

		if (moveList.empty())
		{
			if (b.inCheck(b.isWhiteTurn()))
				return -MAX_EVAL + rootMoves[pvIdx].pvDepth;
			return 0;
		}

		++(rootMoves[pvIdx].pvDepth);

		BoardParser b2;

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveList)
		{
			b2 = BoardParser(b);
			b2.movePiece(move);

			Int score = -abSearch<PV>(b2, e, -beta, -alpha, depth - 1);
			if (score >= beta)
			{
				// beta cutoff
				--(rootMoves[pvIdx].pvDepth);
				return beta;
			}
			if (score > alpha)
			{
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth - 1] = move;
				rootMoveTemp = rootMoves[pvIdx];
				// alpha acts like max in MiniMax
				alpha = score;
				if (rootNode)
				{
					rootMoves[pvIdx].score = score;
				}
			}
			else
			{
				rootMoves[pvIdx] = rootMoveTemp;
			}
			if (rootNode)
			{
				--(rootMoves[pvIdx].pvDepth);
				++pvIdx;
				++(rootMoves[pvIdx].pvDepth);
			}
		}
		if (PvNode)
			--(rootMoves[pvIdx].pvDepth);

		return alpha;
	}

	cMove nextMove(const BoardParser &b, const Evaluate &e) override
	{
		// Checking book
		std::string fen = b.fen();
		std::vector<std::string> movesParsed;
		std::vector<UInt> frequenciesParsed;

		std::ifstream infile("book.txt");
		std::string line, varName, defaultValue;
		std::string delimiter = " ";

		while (std::getline(infile, line) && movesParsed.empty())
		{
			varName = line.substr(0, line.find(delimiter));
			defaultValue = line.substr(line.find(delimiter) + 1);
			if (varName == "pos" && fen == defaultValue)
			{
				while (std::getline(infile, line))
				{
					varName = line.substr(0, line.find(delimiter));
					defaultValue = line.substr(line.find(delimiter) + 1);
					if (line.substr(0, line.find(delimiter)) == "pos")
						break;
					movesParsed.push_back(varName);
					frequenciesParsed.push_back(UInt(std::stoi(defaultValue)));
				}
			}
		}

		if (!movesParsed.empty())
		{
			UInt accMax = std::accumulate(frequenciesParsed.begin(), frequenciesParsed.end(), 0);
			UInt selector = UInt(double(std::rand()) / double(RAND_MAX) * double(accMax));
			UInt acc = 0;
			for (UInt i = 0; i < frequenciesParsed.size(); ++i)
			{
				acc += frequenciesParsed[i];
				if (selector < acc)
					return UCI::to_move(b, movesParsed[i]);
			}
		}

		// Compute rootMoves
		UInt currentDepth = Limits.depth;
		std::vector<cMove> moveList;
		Search::generateMoveList(b, moveList, /*legalOnly=*/ true, false);

		Search::orderMoves(b, moveList);

		if (moveList.empty())
		{
			err("Cannot move.");
			return cMove();
		}
		else if (moveList.size() == 1)
		{
			return moveList[1];
		}

		rootMovesSize = moveList.size();
		for (UInt i = 0; i < rootMovesSize; ++i)
		{
			rootMoves[i] = RootMove(moveList[i]);
		}

		// Iterative deepening algorithm
		while (currentDepth <= Limits.depth)
		{
			// Some variables have to be reset
			for (UInt i = 0; i < rootMovesSize; ++i)
				rootMoves[i].previousScore = rootMoves[i].score;
			nodesSearched.fill(0);
			pvIdx = 0;

			abSearch<Root>(b, e, -MAX_EVAL, MAX_EVAL, currentDepth);
			std::stable_sort(rootMoves.begin(), rootMoves.end());
			std::cout << UCI::pv(*this, b, currentDepth) << std::endl;
			++currentDepth;
		}
		return rootMoves[0].pv[0];
	}
};
