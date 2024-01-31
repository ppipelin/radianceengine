#pragma once

#include "include.h"
#include "evaluate.h"
#include "evaluateShannon.h"
#include "boardParser.h"

#include <algorithm>
#include <cmath>

class EvaluateTable : virtual public EvaluateShannon
{
public:
	EvaluateTable() {}
	EvaluateTable(const EvaluateTable &) {}
	~EvaluateTable() {}

	// Chebyshev distance of kings
	Int distanceKings(const BoardParser &b) const
	{
		Int f1 = Board::column(b.whiteKing());
		Int f2 = Board::column(b.blackKing());
		Int r1 = Board::row(b.whiteKing());
		Int r2 = Board::row(b.blackKing());
		return std::max(std::abs(r2 - r1), std::abs(f2 - f1));
	}

	// Tables are displayed for white which corresponds to black order of tiles
	// https://www.chessprogramming.org/Simplified_Evaluation_Function
	static constexpr std::array<Value, 64> pawnTable = {
		0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5,  5, 10, 25, 25, 10,  5,  5,
		0,  0,  0, 20, 20,  0,  0,  0,
		5, -5,-10,  0,  0,-10, -5,  5,
		5, 10, 10,-20,-20, 10, 10,  5,
		0,  0,  0,  0,  0,  0,  0,  0 };

	static constexpr std::array<Value, 64> knightTable = {
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50 };

	static constexpr std::array<Value, 64> bishopTable = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20 };

	static constexpr std::array<Value, 64> rookTable = {
		0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0 };

	static constexpr std::array<Value, 64> queenTable = {
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-5,  0,  5,  5,  5,  5,  0, -5,
		0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20 };

	static constexpr std::array<Value, 64> kingTable = {
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		20, 20,  0,  0,  0,  0, 20, 20,
		20, 30, 10,  0,  0, 10, 30, 20 };

	static constexpr std::array<Value, 64> kingEndgameTable = {
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50 };

	Value evaluate(const BoardParser &b) const override
	{
		const std::unordered_map<Key, UInt>::const_iterator rep = repetitionTable.find(b.m_materialKey);
		if (rep != repetitionTable.end() && rep->second > 1)
			return 0;

		Value finalScore = 0, scorePieceWhite = 0, scorePieceBlack = 0, scorePieceWhiteTable = 0, scorePieceBlackTable = 0;
		Int movesetWhiteKing = 0, movesetBlackKing = 0;

		for (Int i = -1; i < 2; i += 2)
		{
			std::vector<UInt> table = (i == -1) ? b.boardParsed()->blackPos() : b.boardParsed()->whitePos();
			Value score = 0;
			std::vector<UInt> pawnPositions;
			std::vector<UInt> pawnColumns;

			for (const auto &pieceIdx : table)
			{
				const Piece *p = b.boardParsed()->board()[pieceIdx];

				if (p == nullptr)
					continue;
				if (typeid(*p) == typeid(King))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 20000;
				}
				else if (typeid(*p) == typeid(Queen))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 950;
				}
				else if (typeid(*p) == typeid(Rook))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 563;
				}
				else if (typeid(*p) == typeid(Bishop))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 333;
				}
				else if (typeid(*p) == typeid(Knight))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 305;
				}
				else if (typeid(*p) == typeid(Pawn))
				{
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 100;
					pawnPositions.push_back(pieceIdx);
					pawnColumns.push_back(Board::column(pieceIdx));
				}
				std::vector<cMove> moveset;
				p->canMove(*b.boardParsed(), moveset);
				if (typeid(*p) == typeid(King))
					(i == 1 ? movesetWhiteKing : movesetBlackKing) = Int(moveset.size());
				else
					score += 10 * Value(moveset.size());
			}

			score += (i == 1 ? scorePieceWhite : scorePieceBlack);

			score += pawnMalus(b, pawnPositions, pawnColumns);

			finalScore += i * score;
		}

		// Once ennemy has less pieces our king attacks the other one
		// King, seven pawns a rook and a bishop
		const bool endgame = (b.isWhiteTurn() ? scorePieceBlack : scorePieceWhite) <= 20000 + 7 * 100 + 563 + 333;
		// King, six pawns a bishop and a knight
		const bool endgameHard = (b.isWhiteTurn() ? scorePieceBlack : scorePieceWhite) <= 20000 + 4 * 100 + 333 + 305;

		for (Int i = -1; i < 2; i += 2)
		{
			std::vector<UInt> table = (i == -1) ? b.boardParsed()->blackPos() : b.boardParsed()->whitePos();
			Value score = 0;

			for (const auto &pieceIdx : table)
			{
				const Piece *p = b.boardParsed()->board()[pieceIdx];

				// Find idx in piece-square table
				Int idxTable = i == 1 ? ((BOARD_SIZE - 1) - Board::row(pieceIdx)) * BOARD_SIZE + Board::column(pieceIdx) : pieceIdx;
				if (p == nullptr)
					continue;
				if (typeid(*p) == typeid(King))
				{
					if (endgame)
					{
						if (!endgameHard)
							(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += kingEndgameTable[idxTable];
					}
					else
						(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += kingTable[idxTable];
				}
				else if (typeid(*p) == typeid(Queen))
				{
					(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += queenTable[idxTable];
				}
				else if (typeid(*p) == typeid(Rook))
				{
					(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += rookTable[idxTable];
				}
				else if (typeid(*p) == typeid(Bishop))
				{
					(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += bishopTable[idxTable];
				}
				else if (typeid(*p) == typeid(Knight))
				{
					(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += knightTable[idxTable];
				}
				else if (typeid(*p) == typeid(Pawn))
				{
					(i == 1 ? scorePieceWhiteTable : scorePieceBlackTable) += pawnTable[idxTable];
				}
			}

			score += (i == 1 ? scorePieceWhiteTable : scorePieceBlackTable);

			finalScore += i * score;
		}

		constexpr Value factor = 1;
		if (endgame)
		{
			finalScore += (b.isWhiteTurn() ? 1 : -1) * distanceKings(b);
			finalScore += 1 * factor * movesetWhiteKing;
			finalScore += -1 * factor * movesetBlackKing;
		}
		else
		{
			finalScore += 1 * -factor * movesetWhiteKing;
			finalScore += -1 * -factor * movesetBlackKing;
		}

		return (b.isWhiteTurn() ? 1 : -1) * finalScore;
	}
};
