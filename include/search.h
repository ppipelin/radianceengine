#pragma once

#include "include.h"
#include "cMove.h"
#include "boardParser.h"
#include "evaluate.h"

#include <array>

class Search
{
public:
	struct LimitsType
	{
		LimitsType()
		{
			// Init explicitly due to broken value-initialization of non POD in MSVC
			movestogo = depth = mate = perft = infinite = 0;
			nodes = 0;
		}

		std::vector<cMove> searchmoves;
		int movestogo, depth, mate, perft, infinite;
		int64_t nodes;
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
	UInt rootMovesSize = 0;

	Search(const Search::LimitsType &limits) : Limits(limits) {}
	Search(const Search &) {}
	~Search() {}

	virtual cMove nextMove(const BoardParser &, const Evaluate &)
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

	std::vector<cMove> generateMoveList(const BoardParser &b, std::vector<cMove> &moveList, bool legalOnly = false, bool onlyCapture = false) const
	{
		std::vector<UInt> allyPositions = b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		std::vector<UInt> enemyPositions = !b.isWhiteTurn() ? b.boardParsed()->whitePos() : b.boardParsed()->blackPos();
		for (UInt tileIdx = 0; tileIdx < allyPositions.size(); ++tileIdx)
		{
			UInt tile = allyPositions[tileIdx];
			const Piece *piece = b.boardParsed()->board()[tile];
			if (piece == nullptr || (piece->isWhite() != b.isWhiteTurn()))
			{
				continue;
			}
			if (piece->isWhite() != b.isWhiteTurn())
			{
				warn("is wrong turn");
			}
			std::vector<cMove> subMoveList = std::vector<cMove>();
			piece->canMove(*b.boardParsed(), subMoveList);
			if (onlyCapture)
			{
				std::erase_if(subMoveList, [](cMove move) {return !move.isCapture();});
			}
			moveList.insert(moveList.end(), subMoveList.begin(), subMoveList.end());
		}

		if (legalOnly)
		{
			std::erase_if(moveList, [b](cMove move) {
				BoardParser b2(b);
				b2.movePiece(move);
				// Prune moves which keep the king in check
				return b2.inCheck(b.isWhiteTurn());
				});

			std::erase_if(moveList, [b](cMove move) {
				// Prune moves which castles in check
				return b.inCheck(b.isWhiteTurn()) && move.isCastle();
				});

			std::erase_if(moveList, [b](cMove move) {
				// Prune moves which castles through check
				if (move.getFlags() == 0x2)
				{
					BoardParser b2(b);
					b2.movePiece(cMove(move.getFrom(), move.getFrom() + 1));
					if (b2.inCheck(b.isWhiteTurn())) return true;

					b2 = BoardParser(b);
					b2.movePiece(cMove(move.getFrom(), move.getFrom() + 2));
					if (b2.inCheck(b.isWhiteTurn())) return true;
				}
				else if (move.getFlags() == 0x3)
				{
					BoardParser b2(b);
					b2.movePiece(cMove(move.getFrom(), move.getFrom() - 1));
					if (b2.inCheck(b.isWhiteTurn())) return true;

					b2 = BoardParser(b);
					b2.movePiece(cMove(move.getFrom(), move.getFrom() - 2));
					if (b2.inCheck(b.isWhiteTurn())) return true;

					b2 = BoardParser(b);
					b2.movePiece(cMove(move.getFrom(), move.getFrom() - 3));
					if (b2.inCheck(b.isWhiteTurn())) return true;
				}
				return false;
				});
		}

		return moveList;
	}

	bool inCheckMate(const BoardParser &b, const bool isWhite) const
	{
		if (b.inCheck(isWhite))
		{
			std::vector<cMove> moveList = std::vector<cMove>();
			generateMoveList(b, moveList, true);
			return moveList.empty();
		}
		return false;
	}
};
