#pragma once

#include "include.h"
#include "cMove.h"
#include "boardParser.h"
#include "evaluate.h"

#include <array>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <numeric>

enum Color
{
	WHITE,
	BLACK,
	COLOR_NB = 2
};

using TimePoint = std::chrono::milliseconds::rep; // A value in milliseconds
static_assert(sizeof(TimePoint) == sizeof(int64_t), "TimePoint should be 64 bits");
inline TimePoint now()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::steady_clock::now().time_since_epoch()).count();
}

namespace {
	std::unordered_map<Key, std::pair<Value, UInt>> transpositionTable;
	TimePoint remaining = 0;
}

class Search
{
protected:
	enum NodeType
	{
		NonPV,
		PV,
		Root
	};
	bool *g_stop;
	std::mutex mtx;
public:
	struct LimitsType
	{
		LimitsType()
		{
			// Init explicitly due to broken value-initialization of non POD in MSVC
			movestogo = depth = mate = perft = infinite = nodes = 0;
			time[WHITE] = time[BLACK] = inc[WHITE] = inc[BLACK] = startTime = TimePoint(0);
		}

		std::vector<cMove> searchmoves;
		UInt movestogo, depth, mate, perft, infinite, nodes;
		TimePoint time[COLOR_NB], inc[COLOR_NB], startTime, movetime = 0;
	};

	// RootMove struct is used for moves at the root of the tree. For each root move
	// we store a score and a PV (really a refutation in the case of moves which
	// fail low). Score is normally set at -VALUE_INFINITE for all non-pv moves.
	struct RootMove
	{
		explicit RootMove() : pv{} {}
		explicit RootMove(const cMove m) : pv{ m } {}
		bool operator==(const cMove &m) const { return pv[0] == m; }
		bool operator<(const RootMove &m) const
		{
			// Sort in descending order
			return m.score != score ? m.score < score
				: m.previousScore < previousScore;
		}

		Value score = -VALUE_INFINITE;
		Value previousScore = -VALUE_INFINITE;
		Value averageScore = -VALUE_INFINITE;
		Value uciScore = -VALUE_INFINITE;
		bool scoreLowerbound = false;
		bool scoreUpperbound = false;
		std::vector<cMove> pv;
		UInt pvDepth = 0;
	};

	struct Stack
	{
		cMove *pv;
		UInt ply;
		cMove currentMove;
		cMove excludedMove;
		cMove killers[2];
		Value staticEval;
		UInt moveCount;
		bool inCheck;
		bool ttPv;
		bool ttHit;
		int doubleExtensions;
		int cutoffCnt;
	};

	void update_pv(cMove *pv, cMove move, const cMove *childPv)
	{
		for (*pv++ = move; childPv && *childPv != cMove();)
			*pv++ = *childPv++;
		*pv = cMove();
	}

	LimitsType Limits;
	UInt pvIdx = 0;
	std::vector<RootMove> rootMoves;
	std::array<Int, MAX_PLY> nodesSearched = { 0 };
	UInt transpositionUsed = 0;

	Search(const Search::LimitsType &limits, bool *g_stop) : Limits(limits), g_stop(g_stop) {}
	Search(const Search &) {}
	~Search() {}

	virtual cMove nextMove(const BoardParser &, const Evaluate &)
	{
		return cMove();
	}

	virtual cMove nextMove(BoardParser &, const Evaluate &)
	{
		return cMove();
	}

	cMove probeBook(const BoardParser &b)
	{
		std::string fen = b.fen(/*noEnPassant =*/ false, /*noHalfMove =*/ true);
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
				{
					return b.toMove(movesParsed[i]);
				}
			}
		}
		return cMove();
	}

	static void generateMoveList(BoardParser &b, std::vector<cMove> &moveList, bool legalOnly = false, bool onlyCapture = false)
	{
		std::vector<UInt> allyPositions = b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		std::vector<UInt> enemyPositions = !b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		std::sort(allyPositions.begin(), allyPositions.end());
		std::sort(enemyPositions.begin(), enemyPositions.end());
		moveList.reserve(MAX_PLY);
		for (UInt tileIdx = 0; tileIdx < allyPositions.size(); ++tileIdx)
		{
			UInt tile = allyPositions[tileIdx];
			const Piece *piece = b.boardParsed()->board()[tile];
			piece->canMove(*b.boardParsed(), moveList);
		}

		if (onlyCapture)
		{
			std::erase_if(moveList, [](cMove move) {return !move.isCapture();});
		}

		if (legalOnly)
		{
			// #define optLegalOnly
#ifdef optLegalOnly
			// Look for attacked squares
			std::array<cMove, MAX_PLY> moveListAttack = {};
			size_t moveListAttackSize = 0;
			for (UInt tileIdx = 0; tileIdx < enemyPositions.size(); ++tileIdx)
			{
				UInt tile = enemyPositions[tileIdx];
				const Piece *piece = b.boardParsed()->board()[tile];
				std::vector<cMove> subMoveList;
				piece->canMove(*b.boardParsed(), subMoveList);
				std::copy(subMoveList.begin(), subMoveList.end(), moveListAttack.begin() + moveListAttackSize);
				moveListAttackSize += subMoveList.size();
			}

			std::erase_if(moveList, [b, moveListAttack, moveListAttackSize](cMove move) mutable {
				BoardParser::State s(b);
				b.movePiece(move, s);
				// Prune moves which keep the king in check
				const bool keepInCheck = b.inCheck(!b.isWhiteTurn());
				b.unMovePiece(move, s);
				if (keepInCheck) return true;

				// Prune moves which castles in check
				if (move.isCastle())
					return b.inCheck(b.isWhiteTurn(), moveListAttack, moveListAttackSize);

				// Prune moves which castles through check
				if (move.getFlags() == 0x2)
				{
					return (std::find(moveListAttack.begin(), moveListAttack.begin() + moveListAttackSize, move.getFrom() + 1) != moveListAttack.begin() + moveListAttackSize) ||
						(std::find(moveListAttack.begin(), moveListAttack.begin() + moveListAttackSize, move.getFrom() + 2) != moveListAttack.begin() + moveListAttackSize);
				}
				else if (move.getFlags() == 0x3)
				{
					return (std::find(moveListAttack.begin(), moveListAttack.begin() + moveListAttackSize, move.getFrom() - 1) != moveListAttack.begin() + moveListAttackSize) ||
						(std::find(moveListAttack.begin(), moveListAttack.begin() + moveListAttackSize, move.getFrom() - 2) != moveListAttack.begin() + moveListAttackSize) ||
						(std::find(moveListAttack.begin(), moveListAttack.begin() + moveListAttackSize, move.getFrom() - 3) != moveListAttack.begin() + moveListAttackSize);
				}
				return false;
				});
#endif
#ifndef optLegalOnly
			BoardParser::State s;

			std::erase_if(moveList, [&b, &s](const cMove move) {
#ifdef unMoveTest
				BoardParser::State s2 = *b.m_s;
				BoardParser b2(b, &s2);
#endif
				b.movePiece(move, s);
				// Prune moves which keep the king in check
				bool exit = b.inCheck(!b.isWhiteTurn());
				b.unMovePiece(move);
				s = *b.m_s;
#ifdef unMoveTest
				if (b != b2)
				{
					b.displayCLI();
					std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
				}
				else
					assert(b.m_s->materialKey == b2.m_s->materialKey);
#endif
				if (exit) return true;

				// Prune moves which castles in check
				if (move.isCastle() && b.inCheck(b.isWhiteTurn()))
					return true;

				// Prune moves which castles through check
				if (move.getFlags() == 0x2)
				{
					cMove lastMove = cMove(move.getFrom(), move.getFrom() + 1);
					b.movePiece(lastMove, s);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove);
					s = *b.m_s;
#ifdef unMoveTest
					if (b != b2)
					{
						b.displayCLI();
						std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
					}
#endif
					if (exit)
						return true;

					lastMove = cMove(move.getFrom(), move.getFrom() + 2);
					b.movePiece(lastMove, s);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove);
					s = *b.m_s;
#ifdef unMoveTest
					if (b != b2)
					{
						b.displayCLI();
						std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
					}
#endif
					if (exit)
						return true;
				}
				else if (move.getFlags() == 0x3)
				{
					cMove lastMove = cMove(move.getFrom(), move.getFrom() - 1);
					b.movePiece(lastMove, s);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove);
					s = *b.m_s;
#ifdef unMoveTest
					if (b != b2)
					{
						b.displayCLI();
						std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
					}
#endif
					if (exit)
						return true;

					lastMove = cMove(move.getFrom(), move.getFrom() - 2);
					b.movePiece(lastMove, s);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove);
					s = *b.m_s;
#ifdef unMoveTest
					if (b != b2)
					{
						b.displayCLI();
						std::cout << b.m_s->materialKey << " " << b2.m_s->materialKey << std::endl;
					}
#endif
					if (exit)
						return true;
				}
				return false;
				});
#endif
		}
	}

	struct MoveComparator
	{
		MoveComparator(BoardParser &b) : b(b) {};

		bool operator() (const cMove &m1, const cMove &m2) const
		{
			return (m1.isCapture() && m1.getFlags() != 0x5 ? Int((*b.boardParsed())[m1.getTo()]->value()) - Int((*b.boardParsed())[m1.getFrom()]->value()) : 0) >
				(m2.isCapture() && m2.getFlags() != 0x5 ? Int((*b.boardParsed())[m2.getTo()]->value()) - Int((*b.boardParsed())[m2.getFrom()]->value()) : 0);
		}

		BoardParser &b;
	};

	static void orderMoves(BoardParser &b, std::vector<cMove> &moveList)
	{
		std::sort(moveList.begin(), moveList.end(), MoveComparator(b));
	}

	/**
	* @brief perft test from https://www.chessprogramming.org/Perft
	*
	* @param b
	* @param depth
	* @return UInt number of possibles position after all possible moves on b
	*/
	static UInt perft(BoardParser &b, UInt depth = 1, bool verbose = false)
	{
		std::vector<cMove> moveList;
		UInt nodes = 0;
		std::vector<UInt> tiles;

		if (depth == 0)
		{
			return 1;
		}

		// #define opt
#ifdef opt
		if (b.isWhiteTurn())
		{
			tiles = b.boardParsed()->whitePos();
		}
		else
		{
			tiles = b.boardParsed()->blackPos();
		}
		for (const auto tile : tiles)
		{
			const Piece *piece = (*b.boardParsed())[tile];
			if (piece == nullptr)
			{
				continue;
			}

			piece->canMove(*b.boardParsed(), moveList);
		}
#endif
#ifndef opt
		Search::generateMoveList(b, moveList, /*legalOnly =*/ true);
#endif
		for (const cMove move : moveList)
		{
#ifdef opt
			// Castling
			if (move.isCastle())
			{
				// Castling from a controlled tile is illegal
				if (b.inCheck(b.isWhiteTurn()))
				{
					continue;
				}
				// Castling over a controlled tile is illegal
				cMove moveCastle = cMove(move.getFrom(), move.getTo());
				UInt to = move.getFlags() == 2 ? move.getFrom() + 1 : move.getFrom() - 1;
				BoardParser::State s;
				BoardParser b3 = BoardParser(b, &s);
				moveCastle.setTo(to);
				b3.movePiece(moveCastle);
				if (b3.inCheck(!b3.isWhiteTurn()))
				{
					continue;
				}
				// Queen castle requires another check, keeps last board for performances and re-move king
				if (move.getFlags() == 3)
				{
					moveCastle.setFrom(moveCastle.getTo());
					moveCastle.setTo(move.getFrom() - 2);
					b3.movePiece(moveCastle);
					if (b3.inCheck(!b3.isWhiteTurn()))
					{
						continue;
					}
				}
			}
#endif
			BoardParser::State s;

			if (!b.movePiece(move, s))
			{
				b.unMovePiece(move);
				continue;
			}
#ifdef opt
			// We assert that we are not in check after we just moved
			if (b2.inCheck(!b2.isWhiteTurn()))
			{
				// We don't count illegal moves
				continue;
			}
#endif
			UInt nodesNumber = perft(b, depth - 1);
			b.unMovePiece(move);
			if (verbose)
			{
				std::cout << Board::toString(move.getFrom()) << Board::toString(move.getTo()) << " : " << nodesNumber << std::endl;
			}
			nodes += nodesNumber;
		}
		return nodes;
	}

	template<typename T, typename Predicate>
	void move_to_front(std::vector<T> &vec, Predicate pred)
	{
		auto it = std::find_if(vec.begin(), vec.end(), pred);

		if (it != vec.end())
		{
			std::rotate(vec.begin(), it, it + 1);
		}
	}

	inline TimePoint elapsed() const
	{
		return (now() - Limits.startTime);
	}

	inline bool outOfTime() const
	{
		if (*g_stop)
			return true;
		if (Limits.infinite || remaining == 0) return false;
		return elapsed() > remaining;
	}
};
