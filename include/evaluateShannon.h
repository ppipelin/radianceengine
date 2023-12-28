#pragma once

#include "evaluate.h"
#include "boardParser.h"

#include <algorithm>

class EvaluateShannon : virtual public Evaluate
{
public:
	EvaluateShannon() {}
	EvaluateShannon(const EvaluateShannon &) {}
	~EvaluateShannon() {}

	Int pawnMalus(const BoardParser &b, const std::vector<UInt> &pawnPositions, const std::vector<UInt> &pawnColumns) const
	{
		Int score = 0;
		if (pawnPositions.empty())
		{
			return score;
		}
		// Doubled pawn
		std::vector<UInt> pawnColumsUnique = std::vector<UInt>(pawnColumns);
		std::sort(pawnColumsUnique.begin(), pawnColumsUnique.end());
		const auto last = std::unique(pawnColumsUnique.begin(), pawnColumsUnique.end());
		pawnColumsUnique.erase(last, pawnColumsUnique.end());
		score -= Int(pawnColumns.size() - pawnColumsUnique.size());

		for (const auto &i : pawnPositions)
		{
			const Piece *piece = (*b.boardParsed())[i];
			// Blocked pawn
			// Should never be out of range because a pawn cannot be on a last rank
			if (piece->isWhite() ? (*b.boardParsed())[i + BOARD_SIZE] != nullptr : (*b.boardParsed())[i - BOARD_SIZE] != nullptr)
			{
				score -= 1;
			}

			// Isolated pawn
			UInt pieceColumn = Board::column(i);
			if (std::find(pawnColumns.begin(), pawnColumns.end(), pieceColumn + 1) == pawnColumns.end() && std::find(pawnColumns.begin(), pawnColumns.end(), pieceColumn - 1) == pawnColumns.end())
			{
				score -= 1;
			}
		}
		return score;
	}

	Int evaluate(const BoardParser &b) const override
	{
		// 200(K-K')
		//      + 9(Q-Q')
		//      + 5(R-R')
		//      + 3(B-B' + N-N')
		//      + 1(P-P')
		//      - 0.5(D-D' + S-S' + I-I')
		//      + 0.1(M-M') + ...
		// KQRBNP = number of kings, queens, rooks, bishops, knights and pawns
		// D,S,I = doubled, blocked and isolated pawns
		// M = Mobility (the number of legal moves)
		Int finalScore = 0;
		for (Int i = -1; i < 2; i += 2)
		{
			std::vector<UInt> table;
			if (i == -1)
			{
				table = b.boardParsed()->blackPos();
			}
			else
			{
				table = b.boardParsed()->whitePos();
			}
			Int score = 0;
			std::vector<UInt> pawnPositions;
			std::vector<UInt> pawnColumns;

			for (const auto &pieceIdx : table)
			{
				const Piece *p = b.boardParsed()->board()[pieceIdx];
				if (p == nullptr)
					continue;
				if (typeid(*p) == typeid(King))
					score += 2000;
				else if (typeid(*p) == typeid(Queen))
					score += 90;
				else if (typeid(*p) == typeid(Rook))
					score += 50;
				else if (typeid(*p) == typeid(Bishop))
					score += 30;
				else if (typeid(*p) == typeid(Knight))
					score += 30;
				else if (typeid(*p) == typeid(Pawn))
				{
					score += 10;
					pawnPositions.push_back(pieceIdx);
					pawnColumns.push_back(Board::column(pieceIdx));
				}

				std::vector<cMove> moveset;
				p->canMove(*b.boardParsed(), moveset);
				score += 1 * Int(moveset.size());
			}

			score += pawnMalus(b, pawnPositions, pawnColumns);

			finalScore += i * score;
		}
		return finalScore;
	}
};
