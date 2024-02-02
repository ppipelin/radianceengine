#pragma once

#include "include.h"
#include "cMove.h"
#include "boardParser.h"
#include "evaluate.h"

#include <array>

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

std::unordered_map<Key, std::pair<Value, UInt>> transpositionTable;
std::unordered_map<Key, UInt> repetitionTable;

class Search
{
public:

	inline TimePoint elapsed() const
	{
		return (now() - Limits.startTime);
	}

	inline bool outOfTime(const TimePoint &t) const
	{
		return elapsed() > (t / 30);
	}

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
		TimePoint time[COLOR_NB], inc[COLOR_NB], startTime;
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

		Value score = -MAX_EVAL;
		Value previousScore = -MAX_EVAL;
		Value averageScore = -MAX_EVAL;
		Value uciScore = -MAX_EVAL;
		bool scoreLowerbound = false;
		bool scoreUpperbound = false;
		std::array<cMove, 100> pv;
		UInt pvDepth = 0;
	};

	LimitsType Limits;
	UInt pvIdx = 0;
	std::array<RootMove, MAX_PLY> rootMoves;
	std::array<RootMove, MAX_PLY> rootMovesPrevious;
	std::array<Int, MAX_PLY> nodesSearched = { 0 };
	UInt transpositionUsed = 0;
	UInt rootMovesSize = 0;

	Search(const Search::LimitsType &limits) : Limits(limits) {}
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

	static void generateMoveList(BoardParser &b, std::vector<cMove> &moveList, bool legalOnly = false, bool onlyCapture = false)
	{
		std::vector<UInt> allyPositions = b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		std::vector<UInt> enemyPositions = !b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		moveList.reserve(MAX_PLY);
		for (UInt tileIdx = 0; tileIdx < allyPositions.size(); ++tileIdx)
		{
			UInt tile = allyPositions[tileIdx];
			const Piece *piece = b.boardParsed()->board()[tile];
			std::vector<cMove> subMoveList;
			piece->canMove(*b.boardParsed(), subMoveList);
			moveList.insert(moveList.end(), subMoveList.begin(), subMoveList.end());
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
				b.movePiece(move, &s.lastCapturedPiece);
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
			std::erase_if(moveList, [b](const cMove move) mutable {
				BoardParser::State s(b);
				b.movePiece(move, &s.lastCapturedPiece);
				// Prune moves which keep the king in check
				bool exit = b.inCheck(!b.isWhiteTurn());
				b.unMovePiece(move, s);
				if (exit) return true;

				// Prune moves which castles in check
				if (move.isCastle())
					return b.inCheck(b.isWhiteTurn());

				// Prune moves which castles through check
				if (move.getFlags() == 0x2)
				{
					cMove lastMove = cMove(move.getFrom(), move.getFrom() + 1);
					b.movePiece(lastMove, &s.lastCapturedPiece);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove, s);
					if (exit)
						return true;

					lastMove = cMove(move.getFrom(), move.getFrom() + 2);
					b.movePiece(lastMove, &s.lastCapturedPiece);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove, s);
					if (exit)
						return true;
				}
				else if (move.getFlags() == 0x3)
				{
					cMove lastMove = cMove(move.getFrom(), move.getFrom() - 1);
					b.movePiece(lastMove, &s.lastCapturedPiece);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove, s);
					if (exit)
						return true;

					lastMove = cMove(move.getFrom(), move.getFrom() - 2);
					b.movePiece(lastMove, &s.lastCapturedPiece);
					exit = b.inCheck(!b.isWhiteTurn());
					b.unMovePiece(lastMove, s);
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
		MoveComparator(BoardParser b) { this->b = b; }
		// bool operator() (const cMove &m1, const cMove &m2)
		// {
		// 	if (m1.isCapture() ^ m2.isCapture() || !(m1.isCapture() || m2.isCapture())) return m1.isCapture();
		// 	return (*b.boardParsed())[m1.getTo()]->value() > (*b.boardParsed())[m2.getTo()]->value();
		// }

		bool operator() (const cMove &m1, const cMove &m2)
		{
			return (m1.isCapture() ? Int((*b.boardParsed())[m1.getTo()]->value()) - Int((*b.boardParsed())[m1.getFrom()]->value()) : 0) >
				(m2.isCapture() ? Int((*b.boardParsed())[m2.getTo()]->value()) - Int((*b.boardParsed())[m2.getFrom()]->value()) : 0);
		}

		BoardParser b;
	};

	static void orderMoves(const BoardParser &b, std::vector<cMove> &moveList)
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
			std::vector<cMove> subMoveList;
			const Piece *piece = (*b.boardParsed())[tile];
			if (piece == nullptr)
			{
				continue;
			}

			piece->canMove(*b.boardParsed(), subMoveList);
			moveList.insert(moveList.end(), subMoveList.begin(), subMoveList.end());
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
				BoardParser b3 = BoardParser(b);
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
			BoardParser::State s(b);

			if (!b.movePiece(move, &s.lastCapturedPiece))
			{
				b.unMovePiece(move, s);
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
			b.unMovePiece(move, s);
			if (verbose)
			{
				std::cout << Board::toString(move.getFrom()) << Board::toString(move.getTo()) << " : " << nodesNumber << std::endl;
			}
			nodes += nodesNumber;
		}
		return nodes;
	}
};
