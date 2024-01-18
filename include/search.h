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

class Search
{
public:

	inline TimePoint elapsed() const
	{
		return (now() - Limits.startTime);
	}

	inline bool outOutTime(const TimePoint &t) const
	{
		return elapsed() > (t / 20);
	}

	struct LimitsType
	{
		LimitsType()
		{
			// Init explicitly due to broken value-initialization of non POD in MSVC
			movestogo = depth = mate = perft = infinite = nodes = 0;
			time[WHITE] = time[BLACK] = inc[WHITE] = inc[BLACK] = TimePoint(0);
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

		Int score = -MAX_EVAL;
		Int previousScore = -MAX_EVAL;
		Int averageScore = -MAX_EVAL;
		Int uciScore = -MAX_EVAL;
		bool scoreLowerbound = false;
		bool scoreUpperbound = false;
		std::array<cMove, 100> pv;
		UInt pvDepth = 0;
	};

	LimitsType Limits;
	UInt pvIdx;
	std::array<RootMove, MAX_PLY> rootMoves;
	std::array<RootMove, MAX_PLY> rootMovesPrevious;
	std::array<int, MAX_PLY> nodesSearched;
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

	bool applyMove(BoardParser &b2, const cMove &move) const
	{
		// Castling over a controlled tile is illegal so create temporary board to check path
		if (move.isCastle())
		{
			cMove moveCastle = cMove(move);
			UInt to = move.getFlags() == 2 ? moveCastle.getFrom() + 1 : moveCastle.getFrom() - 1;
			BoardParser b3 = BoardParser(b2);
			moveCastle.setTo(to);
			moveCastle.setFlags(0);
			b3.movePiece(moveCastle);
			if (b3.inCheck(!b3.isWhiteTurn()))
			{
				return false;
			}
		}
		if (!b2.movePiece(move))
		{
			return false;
		}
		// We assert that we are not in check after we just moved
		if (b2.inCheck(!b2.isWhiteTurn()))
		{
			// We don't count illegal moves
			return false;
		}
		return true;
	}

	static std::vector<cMove> generateMoveList(const BoardParser &b, std::vector<cMove> &moveList, bool legalOnly = false, bool onlyCapture = false)
	{
		std::vector<UInt> allyPositions = b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		std::vector<UInt> enemyPositions = !b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		for (UInt tileIdx = 0; tileIdx < allyPositions.size(); ++tileIdx)
		{
			UInt tile = allyPositions[tileIdx];
			const Piece *piece = b.boardParsed()->board()[tile];
			if (piece == nullptr)
			{
				err("nullptr piece");
				continue;
			}
			if (piece->isWhite() != b.isWhiteTurn())
			{
				err("is wrong turn");
				continue;
			}
			std::vector<cMove> subMoveList = std::vector<cMove>();
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
			std::vector<cMove> moveListAttack;
			for (UInt tileIdx = 0; tileIdx < enemyPositions.size(); ++tileIdx)
			{
				UInt tile = enemyPositions[tileIdx];
				const Piece *piece = b.boardParsed()->board()[tile];
				if (piece == nullptr)
				{
					err("nullptr piece");
					continue;
				}
				if (piece->isWhite() == b.isWhiteTurn())
				{
					err("is wrong turn");
					continue;
				}
				std::vector<cMove> subMoveList = std::vector<cMove>();
				piece->canMove(*b.boardParsed(), subMoveList);
				moveListAttack.insert(moveListAttack.end(), subMoveList.begin(), subMoveList.end());
			}
			std::erase_if(moveList, [b](cMove move) {
				BoardParser b2(b);
				b2.movePiece(move);
				// Prune moves which keep the king in check
				return b2.inCheck(b.isWhiteTurn());
				});

			std::erase_if(moveList, [b](cMove move) {
				// Prune moves which castles in check
				return move.isCastle() && b.inCheck(b.isWhiteTurn());
				});

			std::erase_if(moveList, [moveListAttack](cMove move) {
				// Prune moves which castles through check
				if (!move.isCastle())
				{
					return false;
				}
				if (move.getFlags() == 0x2)
				{
					return (std::find(moveListAttack.begin(), moveListAttack.end(), move.getFrom() + 1) != moveListAttack.end()) ||
						(std::find(moveListAttack.begin(), moveListAttack.end(), move.getFrom() + 2) != moveListAttack.end());
				}
				else if (move.getFlags() == 0x3)
				{
					return (std::find(moveListAttack.begin(), moveListAttack.end(), move.getFrom() - 1) != moveListAttack.end()) ||
						(std::find(moveListAttack.begin(), moveListAttack.end(), move.getFrom() - 2) != moveListAttack.end()) ||
						(std::find(moveListAttack.begin(), moveListAttack.end(), move.getFrom() - 3) != moveListAttack.end());
				}
				return false;
				});
#endif
#ifndef optLegalOnly
			std::erase_if(moveList, [b](cMove move) {
				BoardParser b2(b);
				Piece *lastCapturedPiece = nullptr;
				cMove lastMove = move;
				b2.movePiece(lastMove, &lastCapturedPiece);
				// Prune moves which keep the king in check
				if (b2.inCheck(!b2.isWhiteTurn()))
					return true;

				// Prune moves which castles in check
				if (move.isCastle() && b.inCheck(b.isWhiteTurn()))
					return true;

				// Prune moves which castles through check
				if (move.getFlags() == 0x2)
				{
					UInt castleInfo = (b.boardParsed()->m_castleAvailableQueenWhite << 3) | (b.boardParsed()->m_castleAvailableKingWhite << 2) | (b.boardParsed()->m_castleAvailableQueenBlack << 1) | int(b.boardParsed()->m_castleAvailableKingBlack);

					b2.unMovePiece(lastMove, castleInfo, lastCapturedPiece);
					lastMove = cMove(move.getFrom(), move.getFrom() + 1);
					b2.movePiece(lastMove, &lastCapturedPiece);
					if (b2.inCheck(b.isWhiteTurn()))
						return true;
					b2.unMovePiece(lastMove, castleInfo, lastCapturedPiece);

					lastMove = cMove(move.getFrom(), move.getFrom() + 2);
					b2.movePiece(lastMove, &lastCapturedPiece);
					if (b2.inCheck(b.isWhiteTurn()))
						return true;
				}
				else if (move.getFlags() == 0x3)
				{
					UInt castleInfo = (b.boardParsed()->m_castleAvailableQueenWhite << 3) | (b.boardParsed()->m_castleAvailableKingWhite << 2) | (b.boardParsed()->m_castleAvailableQueenBlack << 1) | int(b.boardParsed()->m_castleAvailableKingBlack);

					b2.unMovePiece(lastMove, castleInfo, lastCapturedPiece);
					lastMove = cMove(move.getFrom(), move.getFrom() - 1);
					b2.movePiece(lastMove, &lastCapturedPiece);
					if (b2.inCheck(b.isWhiteTurn()))
						return true;

					b2.unMovePiece(lastMove, castleInfo, lastCapturedPiece);
					lastMove = cMove(move.getFrom(), move.getFrom() - 2);
					b2.movePiece(lastMove, &lastCapturedPiece);
					if (b2.inCheck(b.isWhiteTurn()))
						return true;

					// b2.unMovePiece(lastMove, lastCapturedPiece);
					// lastMove = cMove(move.getFrom(), move.getFrom() - 3);
					// b2.movePiece(lastMove, &lastCapturedPiece);
					// if (b2.inCheck(b.isWhiteTurn()))
					// 	return true;
				}
				return false;
				});
#endif
		}
		return moveList;
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

	static bool inCheckMate(const BoardParser &b, const bool isWhite)
	{
		if (b.inCheck(isWhite))
		{
			std::vector<cMove> moveList = std::vector<cMove>();
			Search::generateMoveList(b, moveList, true);
			return moveList.empty();
		}
		return false;
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
		std::vector<cMove> moveList = std::vector<cMove>();
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
			std::vector<cMove> subMoveList = std::vector<cMove>();
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
			Piece *lastCapturedPiece = nullptr;
			UInt castleInfo = (b.boardParsed()->m_castleAvailableQueenWhite << 3) | (b.boardParsed()->m_castleAvailableKingWhite << 2) | (b.boardParsed()->m_castleAvailableQueenBlack << 1) | int(b.boardParsed()->m_castleAvailableKingBlack);
			if (!b.movePiece(move, &lastCapturedPiece))
			{
				b.unMovePiece(move, castleInfo, lastCapturedPiece);
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
			b.unMovePiece(move, castleInfo, lastCapturedPiece);
			if (verbose)
			{
				std::cout << Board::toString(move.getFrom()) << Board::toString(move.getTo()) << " : " << nodesNumber << std::endl;
			}
			nodes += nodesNumber;
		}
		return nodes;
	}
};
