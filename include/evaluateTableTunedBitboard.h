#pragma once

#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "boardParser.h"
#include "evaluate.h"
#include "evaluateShannonHeuristics.h"
#include "evaluateTableTuned.h"
#include "include.h"

class EvaluateTableTunedBitboard : virtual public EvaluateTableTuned
{
public:
	EvaluateTableTunedBitboard() = default;
	EvaluateTableTunedBitboard(const EvaluateTableTunedBitboard &) = default;
	~EvaluateTableTunedBitboard() = default;

	Value evaluate(const BoardParser &b) const override
	{
		Value finalScore = 0, scorePieceWhite = 0, scorePieceBlack = 0, mgScore = 0, egScore = 0, scoreKingWhite = 0, scoreKingBlack = 0;
		std::vector<cMove> moveset;

		// Helper lambda to evaluate a given piece type
		auto evaluatePiece = [&](PieceType pieceType, Value baseValue, const Value pieceTable[64]) {
			Bitboard whitePieces = Bitboards::bbPieces[pieceType] & Bitboards::bbColors[Color::WHITE];
			Bitboard blackPieces = Bitboards::bbPieces[pieceType] & Bitboards::bbColors[Color::BLACK];

			// Evaluate white pieces
			while (whitePieces)
			{
				const UInt pieceIdx = Bitboards::popLeastBit(whitePieces);
				const UInt idxTable = ((BOARD_SIZE - 1) - Board::row(pieceIdx)) * BOARD_SIZE + Board::column(pieceIdx);

				if (pieceType == PieceType::KING)
				{
					scorePieceWhite += 20000;
					moveset.clear();
					b.boardParsed()->board()[pieceIdx]->canMove(*b.boardParsed(), moveset);
					egScore += Value(moveset.size());
					scoreKingWhite = kingEndgameTable[idxTable];
					egScore += scoreKingWhite;
					mgScore += (kingTable[idxTable] - Value(moveset.size()));
				}
				else
				{
					scorePieceWhite += baseValue;
					const Value v = pieceTable[idxTable];
					mgScore += v;
					egScore += v;
				}
			}

			// Evaluate black pieces
			while (blackPieces)
			{
				const UInt pieceIdx = Bitboards::popLeastBit(blackPieces);
				const UInt idxTable = pieceIdx;

				if (pieceType == PieceType::KING)
				{
					scorePieceBlack += 20000;
					moveset.clear();
					b.boardParsed()->board()[pieceIdx]->canMove(*b.boardParsed(), moveset);
					egScore -= Value(moveset.size());
					scoreKingBlack = kingEndgameTable[idxTable];
					egScore -= scoreKingBlack;
					mgScore -= (kingTable[idxTable] - Value(moveset.size()));
				}
				else
				{
					scorePieceBlack += baseValue;
					const Value v = pieceTable[idxTable];
					mgScore -= v;
					egScore -= v;
				}
			}
			finalScore += scorePieceWhite - scorePieceBlack;
			};

		// Evaluate passed, isolated, and doubled pawns
		auto evaluatePawns = [&](Color color) {
			Bitboard pawns = Bitboards::bbPieces[PieceType::PAWN] & Bitboards::bbColors[color];
			Value *scoreCurrent = (color == Color::WHITE) ? &scorePieceWhite : &scorePieceBlack;
			while (pawns)
			{
				const UInt pieceIdx = Bitboards::popLeastBit(pawns);
				Bitboard f = filterPassedPawn(pieceIdx, color);
				*scoreCurrent += passedPawnTable[color == Color::WHITE ? Board::row(pieceIdx) : BOARD_SIZE - 1 - Board::row(pieceIdx)] * Value((f & Bitboards::bbPieces[PieceType::PAWN] & Bitboards::bbColors[!color]) == 0);

				Bitboard f2 = filterIsolatedPawn(pieceIdx);
				*scoreCurrent -= 50 * Value((f2 & Bitboards::bbPieces[PieceType::PAWN] & Bitboards::bbColors[color] & ~Bitboards::tileToBB(pieceIdx)) == 0);

				Bitboard f3 = filterDoubledPawn(pieceIdx);
				*scoreCurrent -= 20 * Value((f3 & Bitboards::bbPieces[PieceType::PAWN] & Bitboards::bbColors[color] & ~Bitboards::tileToBB(pieceIdx)) == 0);
			}
			};

		// Evaluate each piece type
		evaluatePiece(PieceType::KING, 20000, kingTable.data());
		evaluatePiece(PieceType::QUEEN, 950, queenTable.data());
		evaluatePiece(PieceType::ROOK, 563, rookTable.data());
		evaluatePiece(PieceType::BISHOP, 333, bishopTable.data());
		evaluatePiece(PieceType::KNIGHT, 305, knightTable.data());
		evaluatePiece(PieceType::PAWN, 100, pawnTable.data());
		evaluatePawns(Color::WHITE);
		evaluatePawns(Color::BLACK);

		// Final score adjustments based on the endgame condition
		const bool endgame = (b.isWhiteTurn() ? scorePieceBlack : scorePieceWhite) <= 20000 + 7 * 100 + 563 + 333;
		if (endgame)
		{
			finalScore += (b.isWhiteTurn() ? 1 : -1) * distanceKings(b);
			if (scorePieceWhite > scorePieceBlack)
				finalScore += -scoreKingBlack;
			else if (scorePieceWhite < scorePieceBlack)
				finalScore += scoreKingWhite;
			finalScore += egScore;
		}
		else
		{
			finalScore += mgScore;
		}

		return (b.isWhiteTurn() ? 1 : -1) * finalScore;
	}
};
