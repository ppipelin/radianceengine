#pragma once

#include "include.h"
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

	// #define unMoveTest
	// #define transposition

	Value quiesce(BoardParser &b, const Evaluate &e, Value alpha, Value beta)
	{
		++nodesSearched[pvIdx];

		// In order to get the quiescence search to terminate, plies are usually restricted to moves that deal directly with the threat,
		// such as moves that capture and recapture (often called a 'capture search') in chess
		Value stand_pat = e.evaluate(b);
		if (stand_pat >= beta)
			return beta;
		if (alpha < stand_pat)
			alpha = stand_pat;

		// Early quit for quiesce
		// if (rootMoves[pvIdx].pvDepth >= 12)
		// 	return alpha;

		std::vector<cMove> moveListCaptures;
		Search::generateMoveList(b, moveListCaptures, /*legalOnly=*/ true, true);

		Search::orderMoves(b, moveListCaptures);

		// Quiet position
		if (moveListCaptures.empty())
		{
			if (b.inCheck(b.isWhiteTurn()))
				return -VALUE_MATE + rootMoves[pvIdx].pvDepth;
			return alpha;
		}

		++(rootMoves[pvIdx].pvDepth);

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveListCaptures)
		{
			BoardParser::State s(b);

#ifdef unMoveTest
			BoardParser b2(b);
#endif
			b.movePiece(move, &s.lastCapturedPiece);
			Value score = -quiesce(b, e, -beta, -alpha);
			b.unMovePiece(move, s);
#ifdef unMoveTest
			if (b != b2)
			{
				b.displayCLI();
				std::cout << b.m_materialKey << " " << b2.m_materialKey << std::endl;
			}
#endif

			if (score >= beta)
			{
				// beta cutoff
				rootMoves[pvIdx] = rootMoveTemp;
				--(rootMoves[pvIdx].pvDepth);
				return beta;
			}
			if (score > alpha)
			{
				// rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth - 1] = move;
				rootMoveTemp = rootMoves[pvIdx];
				// alpha acts like max in MiniMax
				alpha = score;
			}
			else
			{
				rootMoves[pvIdx] = rootMoveTemp;
			}
			TimePoint t(b.isWhiteTurn() ? Limits.time[WHITE] : Limits.time[BLACK]);
			if (t && outOfTime(t))
				break;
		}
		--(rootMoves[pvIdx].pvDepth);
		return alpha;
	}

	// https://www.chessprogramming.org/Alpha-Beta
	template <NodeType nodeType>
	Value abSearch(BoardParser &b, const Evaluate &e, Value alpha, Value beta, UInt depth)
	{
		++nodesSearched[pvIdx];

		constexpr bool PvNode = nodeType != NonPV;
		constexpr bool rootNode = nodeType == Root;

		std::vector<cMove> moveList;
		if (depth <= 0)
		{
			// return e.evaluate(b);
			return quiesce(b, e, alpha, beta);
		}
		if (rootNode)
		{
			for (UInt i = 0; i < rootMovesSize; ++i)
				moveList.push_back(rootMoves[i].pv[0]);
			// Search::orderMoves(b, moveList);
		}
		else
		{
			Search::generateMoveList(b, moveList, /*legalOnly=*/ true, false);

			Search::orderMoves(b, moveList);

			if (moveList.empty())
			{
				if (b.inCheck(b.isWhiteTurn()))
					return -VALUE_MATE + rootMoves[pvIdx].pvDepth;
				return 0;
			}
		}

		if (!rootNode)
			++(rootMoves[pvIdx].pvDepth);

		RootMove rootMoveTemp = rootMoves[pvIdx];
		for (const cMove move : moveList)
		{
			if (rootNode)
			{
				++(rootMoves[pvIdx].pvDepth);
				rootMoveTemp = rootMoves[pvIdx];
			}
			Value score = VALUE_NONE;
#ifdef unMoveTest
			BoardParser b2(b);
#endif
			BoardParser::State s(b);
			b.movePiece(move, &s.lastCapturedPiece);
#ifdef transposition
			auto it = transpositionTable.find(b.m_materialKey);
			const bool found = it != transpositionTable.end();
			if (found && it->second.second > depth - 1)
			{
				++transpositionUsed;
				score = it->second.first;
			}
			else
			{
#endif
				// LMR
				if (depth >= 2 && pvIdx > 3 && !move.isCapture() && !move.isPromotion() && !b.inCheck(b.isWhiteTurn()))
				{
					// Reduced LMR
					UInt d(std::max(1, Int(depth) - 4));
					score = -abSearch<NonPV>(b, e, -(alpha + 1), -alpha, d - 1);
					// Failed so roll back to full-depth null window
					if (score > alpha && depth > d)
					{
						score = -abSearch<NonPV>(b, e, -(alpha + 1), -alpha, depth - 1);
					}
				}
				// Full-depth null window search when LMR is skipped
				else if (!PvNode)
				{
					score = -abSearch<NonPV>(b, e, -(alpha + 1), -alpha, depth - 1);
				}

				if (PvNode && score > alpha)
				{
					score = -abSearch<PV>(b, e, -beta, -alpha, depth - 1);
				}
#ifdef transposition
			}
			if (!found)
				transpositionTable[b.m_materialKey] = std::pair<Value, UInt>(score, depth - 1);
			else if (it->second.second <= depth - 1)
				it->second = std::pair<Value, UInt>(score, depth - 1);
#endif

			b.unMovePiece(move, s);
#ifdef unMoveTest
			if (b != b2)
			{
				b.displayCLI();
				std::cout << b.m_materialKey << " " << b2.m_materialKey << std::endl;
			}
#endif

			if (score >= beta)
			{
				// beta cutoff
				rootMoves[pvIdx] = rootMoveTemp;
				--(rootMoves[pvIdx].pvDepth);
				return beta;
			}
			if (score > alpha)
			{
				rootMoves[pvIdx].pv[rootMoves[pvIdx].pvDepth - 1] = move;
				// alpha acts like max in MiniMax
				alpha = score;
				if (rootNode)
				{
					rootMoves[pvIdx].score = score;
					rootMoves[pvIdx].averageScore = rootMoves[pvIdx].previousScore != -VALUE_INFINITE ? (2 * rootMoves[pvIdx].score + rootMoves[pvIdx].previousScore) / 3 : rootMoves[pvIdx].score;
				}
				rootMoveTemp = rootMoves[pvIdx];
			}
			else
			{
				rootMoves[pvIdx] = rootMoveTemp;
			}
			if (rootNode)
			{
				--(rootMoves[pvIdx].pvDepth);
				TimePoint t(b.isWhiteTurn() ? Limits.time[WHITE] : Limits.time[BLACK]);
				if (t && outOfTime(t) && depth > 1)
					return -VALUE_NONE;
				++pvIdx;
			}
		}
		if (!rootNode)
			--(rootMoves[pvIdx].pvDepth);

		return alpha;
	}

	cMove nextMove(BoardParser &b, const Evaluate &e) override
	{
		// Checking book
		std::string fen = b.fen(/*noEnPassant =*/ true);
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
		UInt currentDepth = 1;
		transpositionTable.clear(); // not necessary but provide more consistente UCI::pv
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
			return moveList[0];
		}

		rootMovesSize = UInt(moveList.size());
		for (UInt i = 0; i < rootMovesSize; ++i)
		{
			rootMoves[i] = RootMove(moveList[i]);
		}

		// Iterative deepening algorithm
		std::copy(rootMoves.begin(), rootMoves.begin() + rootMovesSize, rootMovesPrevious.begin());
		while (true)
		{
			// Some variables have to be reset
			for (UInt i = 0; i < rootMovesSize; ++i)
			{
				rootMoves[i].previousScore = std::move(rootMoves[i].score);
				rootMoves[i].score = -VALUE_INFINITE;
			}

			// Reset aspiration window starting size
			Value prev = rootMoves[0].averageScore;
			Value delta = std::abs(prev / 2);
			Value alpha = std::max(prev - delta, -VALUE_INFINITE);
			Value beta = std::min(prev + delta, VALUE_INFINITE);
			Value failedHighCnt = 0;
			// Aspiration window
			// Disable by alpha = -VALUE_INFINITE; beta = VALUE_INFINITE;
			alpha = -VALUE_INFINITE; beta = VALUE_INFINITE;
			while (true)
			{
				nodesSearched.fill(0);
				pvIdx = 0;

#ifdef unMoveTest
				BoardParser b2(b);
#endif
				Value score = abSearch<Root>(b, e, alpha, beta, currentDepth);
#ifdef unMoveTest
				if (b != b2)
				{
					b.displayCLI();
					std::cout << b.m_materialKey << " " << b2.m_materialKey << std::endl;
				}
#endif

				TimePoint t(b.isWhiteTurn() ? Limits.time[WHITE] : Limits.time[BLACK]);
				if (t && outOfTime(t) && currentDepth > 1)
					break;

				// In case of failing low/high increase aspiration window and
				// re-search, otherwise exit the loop.
				if (score <= alpha)
				{
					beta = (alpha + beta) / 2;
					alpha = std::max(score - delta, -VALUE_INFINITE);
					failedHighCnt = 0;
					std::copy(rootMovesPrevious.begin(), rootMovesPrevious.begin() + rootMovesSize, rootMoves.begin());
				}
				else if (score >= beta)
				{
					beta = std::min(score + delta, VALUE_INFINITE);
					++failedHighCnt;
				}
				else
					break;

				std::stable_sort(rootMoves.begin(), rootMoves.begin() + rootMovesSize);

				delta += delta / 3;
			}
			// Incomplete search rollback
			TimePoint t(b.isWhiteTurn() ? Limits.time[WHITE] : Limits.time[BLACK]);
			if (t && outOfTime(t) && currentDepth > 1)
			{
				std::copy(rootMovesPrevious.begin(), rootMovesPrevious.begin() + rootMovesSize, rootMoves.begin());
				break;
			}

			std::stable_sort(rootMoves.begin(), rootMoves.begin() + rootMovesSize);
			std::copy(rootMoves.begin(), rootMoves.begin() + rootMovesSize, rootMovesPrevious.begin());
			std::cout << "info failedHighCnt " << failedHighCnt << " alpha " << alpha << " beta " << beta << std::endl;
			std::cout << UCI::pv(*this, currentDepth) << std::endl;

			++currentDepth;

			if (Limits.depth && currentDepth > Limits.depth)
				break;
		}
		return rootMoves[0].pv[0];
	}
};
