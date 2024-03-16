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

	template<NodeType nodeType>
	Value quiesce(BoardParser &b, const Evaluate &e, Stack *ss, Value alpha, Value beta)
	{
		++nodesSearched[pvIdx];

		static_assert(nodeType != Root);
		constexpr bool pvNode = nodeType == PV;

		// // Check if we have an upcoming move that draws by repetition, or
		// // if the opponent had an alternative move earlier to this position.
		// if (alpha < VALUE_DRAW && pos.has_game_cycle(ss->ply))
		// {
		// 	alpha = value_draw(this->nodes);
		// 	if (alpha >= beta)
		// 		return alpha;
		// }

		cMove pv[MAX_PLY + 1];
		Value score = -VALUE_INFINITE, bestScore = -VALUE_INFINITE;
		UInt moveCount = 0;

		if (pvNode)
		{
			(ss + 1)->pv = pv;
			ss->pv[0] = cMove();
		}

		// Used to send selDepth info to GUI (selDepth counts from 1, ply from 0)
#pragma warning( disable: 4127 )
		if (pvNode && selDepth < ss->ply + 1)
			selDepth = ss->ply + 1;
#pragma warning( default: 4127 )

		// In order to get the quiescence search to terminate, plies are usually restricted to moves that deal directly with the threat,
		// such as moves that capture and recapture (often called a 'capture search') in chess
		// Stand pat
		bestScore = e.evaluate(b);
		// ss->inCheck = b.inCheck(b.isWhiteTurn());

		// Check for an immediate draw or maximum ply reached
		// if (/* pos.is_draw(ss->ply) || */ ss->ply >= MAX_PLY)
		// 	return (ss->ply >= MAX_PLY && !ss->inCheck) ? bestScore : VALUE_DRAW;

		// if (ss->inCheck)
		// 	bestScore = -VALUE_INFINITE;
		// else
		// {
		if (bestScore >= beta)
			return bestScore;
		if (alpha < bestScore)
			alpha = bestScore;
		// }

		// Early quit for quiesce
		// if (rootMoves[pvIdx].pvDepth >= 12)
		// 	return alpha;

		std::vector<cMove> moveListCaptures;
		Search::generateMoveList(b, moveListCaptures, /*legalOnly=*/ true, true);

		Search::orderMoves(b, moveListCaptures);

		// Quiet position
		if (moveListCaptures.empty())
		{
			if (b.inCheck(b.isWhiteTurn())) // (ss->inCheck)
				return mated_in(ss->ply);
			return bestScore;
		}

		for (const cMove move : moveListCaptures)
		{
			BoardParser::State s;
			++moveCount;
			ss->currentMove = move;
#ifdef unMoveTest
			BoardParser::State s2 = *b.m_s;
			BoardParser b2(b, &s2);
#endif
			b.movePiece(move, s);
			score = -quiesce<nodeType>(b, e, ss + 1, -beta, -alpha);
			b.unMovePiece(move);
#ifdef unMoveTest
			if (b != b2)
			{
				b.displayCLI();
				std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
			}
#endif

			if (score > bestScore)
			{
				bestScore = score;
				if (score > alpha)
				{
					// Update pv even in fail-high case
					if (pvNode)
						update_pv(ss->pv, move, (ss + 1)->pv);
					if (score >= beta)
					{
						// beta cutoff
						return score;
					}
					// alpha acts like max in MiniMax
					alpha = score;
				}
			}

			if (outOfTime())
				break;
		}

		// All legal moves have been searched. A special case: if we're in check
		// and no legal moves were found, it is checkmate.
		if (ss->inCheck && bestScore == -VALUE_INFINITE)
		{
			// Plies to mate from the root
			return mated_in(ss->ply);
		}

		if (std::abs(bestScore) < VALUE_TB_WIN_IN_MAX_PLY && bestScore >= beta)
			bestScore = (3 * bestScore + beta) / 4;

		return bestScore;
	}

	// https://www.chessprogramming.org/Alpha-Beta
	template <NodeType nodeType>
	Value abSearch(BoardParser &b, const Evaluate &e, Stack *ss, Value alpha, Value beta, UInt depth)
	{
		++nodesSearched[pvIdx];

		constexpr bool pvNode = nodeType != NonPV;
		constexpr bool rootNode = nodeType == Root;

		std::vector<cMove> moveList;
		if (depth <= 0)
		{
			// return e.evaluate(b);
			return quiesce<pvNode ? PV : NonPV>(b, e, ss, alpha, beta);
		}

		// // Check if we have an upcoming move that draws by repetition, or
		// // if the opponent had an alternative move earlier to this position.
		// if (!rootNode && alpha < VALUE_DRAW && b.has_game_cycle(ss->ply))
		// {
		// 	alpha = value_draw(this->nodes);
		// 	if (alpha >= beta)
		// 		return alpha;
		// }

		Value score = -VALUE_INFINITE, bestScore = -VALUE_INFINITE;
		cMove pv[MAX_PLY + 1];
		UInt moveCount = ss->moveCount = 0;

		// Used to send selDepth info to GUI (selDepth counts from 1, ply from 0)
#pragma warning( disable: 4127 )
		if (pvNode && selDepth < ss->ply + 1)
			selDepth = ss->ply + 1;
#pragma warning( default: 4127 )
		if (rootNode)
		{
			for (UInt i = 0; i < rootMoves.size(); ++i)
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
					return mated_in(ss->ply);
				// Stalemate
				return VALUE_DRAW;
			}

			// Step 3. Mate distance pruning. Even if we mate at the next move our score
			// would be at best mate_in(ss->ply + 1), but if alpha is already bigger because
			// a shorter mate was found upward in the tree then there is no need to search
			// because we will never beat the current alpha. Same logic but with reversed
			// signs apply also in the opposite condition of being mated instead of giving
			// mate. In this case, return a fail-high score.
			alpha = std::max(mated_in(ss->ply), alpha);
			beta = std::min(mate_in(ss->ply + 1), beta);
			if (alpha >= beta)
				return alpha;
		}

		(ss + 1)->excludedMove = cMove();
		(ss + 2)->killers[0] = (ss + 2)->killers[1] = cMove();
		(ss + 2)->cutoffCnt = 0;

		// cMove ttMove = rootNode ? rootMoves[pvIdx].pv[0] : cMove();

		// At non-PV nodes we could check for an early TT cutoff
		// Possible reduction/extension here, make sure to quiesce() again afterward

		for (const cMove move : moveList)
		{
			BoardParser::State s;
			// MultiPV future support
			// Cannot find move from rootMoves + pvIdx to the end
#pragma warning( disable: 4127 )
			if (rootNode && std::find(rootMoves.begin() + pvIdx, rootMoves.end(), move) == rootMoves.end())
				continue;
#pragma warning( default: 4127 )

			ss->moveCount = ++moveCount;

			if (pvNode)
				(ss + 1)->pv = nullptr;

			// Possible reduction/extension here

#ifdef unMoveTest
			BoardParser::State s2 = *b.m_s;
			BoardParser b2(b, &s2);
#endif
			ss->currentMove = move;
			b.movePiece(move, s);
			if (!rootNode && b.m_s->repetition && b.m_s->repetition < 0)
				score = VALUE_DRAW;
			else
			{
#ifdef transposition
				auto it = transpositionTable.find(b.m_s->materialKey);
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
						// Reduced-depth LMR
						UInt d(std::max(1, Int(depth) - 4));
						score = -abSearch<NonPV>(b, e, ss + 1, -(alpha + 1), -alpha, d - 1);
						// Failed so roll back to full-depth null window
						if (score > alpha && depth > d)
						{
							score = -abSearch<NonPV>(b, e, ss + 1, -(alpha + 1), -alpha, depth - 1);
						}
					}
					// Full-depth null window search when LMR is skipped
#pragma warning( disable: 4127 )
					else if (!pvNode || moveCount > 1)
					{
						score = -abSearch<NonPV>(b, e, ss + 1, -(alpha + 1), -alpha, depth - 1);
					}

					if (pvNode && (moveCount == 1 || score > alpha))
					{
						(ss + 1)->pv = pv;
						(ss + 1)->pv[0] = cMove();
						score = -abSearch<PV>(b, e, ss + 1, -beta, -alpha, depth - 1);
					}
#pragma warning( default: 4127 )
#ifdef transposition
				}
				if (!found)
					transpositionTable[b.m_s->materialKey] = std::pair<Value, UInt>(score, depth - 1);
				else if (it->second.second <= depth - 1)
					it->second = std::pair<Value, UInt>(score, depth - 1);
#endif
				b.unMovePiece(move);
#ifdef unMoveTest
				if (b != b2)
				{
					b.displayCLI();
					std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
				}
#endif

				if (outOfTime() && depth > 1)
				{
					return -VALUE_NONE;
				}

				if (rootNode)
				{
					RootMove &rm =
						*std::find(rootMoves.begin(), rootMoves.end(), move);

					rm.averageScore = rm.averageScore != -VALUE_INFINITE ? (2 * score + rm.averageScore) / 3 : score;

					// PV move or new best move?
					if (moveCount == 1 || score > alpha)
					{
						rm.score = score;
						rm.selDepth = selDepth;
						rm.scoreLowerbound = rm.scoreUpperbound = false;

						if (score >= beta)
						{
							rm.scoreLowerbound = true;
							rm.uciScore = beta;
						}
						else if (score <= alpha)
						{
							rm.scoreUpperbound = true;
							rm.uciScore = alpha;
						}

						rm.pv.resize(1);

						assert((ss + 1)->pv);

						for (cMove *m = (ss + 1)->pv; *m != cMove(); ++m)
							rm.pv.push_back(*m);

						// We record how often the best move has been changed in each iteration.
						// This information is used for time management. In MultiPV mode,
						// we must take care to only do this for the first PV line.
						if (moveCount > 1 && !pvIdx)
							++bestMoveChanges;
					}
					else
						// All other moves but the PV, are set to the lowest value: this
						// is not a problem when sorting because the sort is stable and the
						// move position in the list is preserved - just the PV is pushed up.
						rm.score = -VALUE_INFINITE;
				}

				if (score > bestScore)
				{
					bestScore = score;
					if (score > alpha)
					{
						// Update pv even in fail-high case
						if constexpr (pvNode && !rootNode)
							update_pv(ss->pv, move, (ss + 1)->pv);

						if (score >= beta)
						{
							// beta cutoff
							++(ss->cutoffCnt);
							break;
						}
						else
						{
							// // Reduce other moves if we have found at least one score improvement (~2 Elo)
							// if (depth > 2 && depth < 12) // && beta < 13195 && value > -12346
							// 	depth -= 2;
							alpha = score;
						}
					}
				}
			}
		}
		// if (!moveCount)
		// 	bestScore = ss->inCheck ? mated_in(ss->ply) : VALUE_DRAW;
		// else if (bestMove != cMove())
		// {

		// }
		// if (PvNode)
		// 	bestScore = std::min(bestScore, maxValue);
		return bestScore;
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

		remaining = TimePoint(b.isWhiteTurn() ? Limits.time[WHITE] : Limits.time[BLACK]);

		// Compute rootMoves
		std::vector<cMove> lastBestPV = { cMove() };
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

		for (UInt i = 0; i < moveList.size(); ++i)
		{
			rootMoves.emplace_back(moveList[i]);
		}

		// Allocate stack with extra size to allow access from (ss - 7) to (ss + 2):
		// (ss - 7) is needed for update_continuation_histories(ss - 1) which accesses (ss - 6),
		// (ss + 2) is needed for initialization of cutOffCnt and killers.
		Stack stack[MAX_PLY + 10], *ss = stack + 7;
		cMove pv[MAX_PLY + 1];

		for (int i = 0; i <= MAX_PLY + 2; ++i)
			(ss + i)->ply = i;

		ss->pv = pv;

		Value bestScore = -VALUE_INFINITE, lastBestScore = -VALUE_INFINITE;


		// Iterative deepening algorithm
		for (; currentDepth < MAX_PLY && !(Limits.depth && currentDepth > Limits.depth); ++currentDepth)
		{
			// Some variables have to be reset
			bestMoveChanges = 0;
			for (RootMove &rm : rootMoves)
			{
				rm.previousScore = std::move(rm.score);
				rm.score = -VALUE_INFINITE;
			}

			// Reset aspiration window starting size
			Value prev = rootMoves[0].averageScore;
			Value delta = std::abs(prev / 2);
			Value alpha = std::max(prev - delta, -VALUE_INFINITE);
			Value beta = std::min(prev + delta, VALUE_INFINITE);
			Value failedHighCnt = 0;
			pvIdx = 0;
			// Aspiration window
			// Disable by alpha = -VALUE_INFINITE; beta = VALUE_INFINITE;
			// alpha = -VALUE_INFINITE; beta = VALUE_INFINITE;
			// Can support MultiPV later on
			while (true)
			{
				// Reset UCI info for each depth and each PV line
				nodesSearched.fill(0);
				selDepth = 0;
				// pvIdx = 0; TO CHECK

#ifdef unMoveTest
				BoardParser::State s2 = *b.m_s;
				BoardParser b2(b, &s2);
#endif
				bestScore = abSearch<Root>(b, e, ss, alpha, beta, currentDepth);
#ifdef unMoveTest
				if (b != b2)
				{
					b.displayCLI();
					std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
				}
#endif

				if (outOfTime() && currentDepth > 1)
					break;

				// MultiPV only orders nonPV
				std::stable_sort(rootMoves.begin() + pvIdx, rootMoves.end());

				// In case of failing low/high increase aspiration window and
				// re-search, otherwise exit the loop.
				if (bestScore <= alpha)
				{
					beta = (alpha + beta) / 2;
					alpha = std::max(bestScore - delta, -VALUE_INFINITE);
					failedHighCnt = 0;
				}
				else if (bestScore >= beta)
				{
					beta = std::min(bestScore + delta, VALUE_INFINITE);
					++failedHighCnt;
				}
				else
					break;

				delta += delta / 3;
			}
			// Incomplete search rollback
			if (outOfTime() && currentDepth > 1)
			{
				// Bring the last best move to the front for best thread selection.
				move_to_front(rootMoves, [&lastBestPV = std::as_const(lastBestPV)](const auto &rm) { return rm == lastBestPV[0]; });
				rootMoves[0].pv = lastBestPV;
				rootMoves[0].score = rootMoves[0].uciScore = lastBestScore;
				break;
			}
			else if (rootMoves[0].pv[0] != lastBestPV[0])
			{
				lastBestPV = rootMoves[0].pv;
				lastBestScore = rootMoves[0].score;
			}

			// Sort the PV lines searched so far and update the GUI
			// std::stable_sort(rootMoves.begin() + pvFirst, rootMoves.begin() + pvIdx + 1);

			std::cout << "info bestMoveChanges " << bestMoveChanges << " failedHighCnt " << failedHighCnt << " alpha " << alpha << " beta " << beta << std::endl;
			std::cout << UCI::pv(*this, currentDepth) << std::endl;
		}
		return rootMoves[0].pv[0];
	}
};
