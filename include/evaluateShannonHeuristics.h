#pragma once

#include "include.h"
#include "evaluateShannon.h"
#include "evaluate.h"
#include "boardParser.h"

#include <algorithm>
#include <cmath>

class EvaluateShannonHeuristics : virtual public EvaluateShannon
{
public:
	EvaluateShannonHeuristics() {}
	EvaluateShannonHeuristics(const EvaluateShannonHeuristics &) {}
	~EvaluateShannonHeuristics() {}

	// Chebyshev distance of kings
	Int distanceKings(const BoardParser &b) const
	{
		Int f1 = Board::column(b.whiteKing());
		Int f2 = Board::column(b.blackKing());
		Int r1 = Board::row(b.whiteKing());
		Int r2 = Board::row(b.blackKing());
		return std::max(std::abs(r2 - r1), std::abs(f2 - f1));
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
		Int finalScore = 0, scorePieceWhite = 0, scorePieceBlack = 0;
		Int movesetWhiteKing = 0, movesetBlackKing = 0;

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
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 20000;
				else if (typeid(*p) == typeid(Queen))
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 950;
				else if (typeid(*p) == typeid(Rook))
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 563;
				else if (typeid(*p) == typeid(Bishop))
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 333;
				else if (typeid(*p) == typeid(Knight))
					(i == 1 ? scorePieceWhite : scorePieceBlack) += 305;
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
					score += 10 * Int(moveset.size());
			}

			score += (i == 1 ? scorePieceWhite : scorePieceBlack);

			score += pawnMalus(b, pawnPositions, pawnColumns);

			finalScore += i * score;
		}

		// Once ennemy has less pieces our king attacks the other one
		// Threshold defined as the value of a king, six pawns a bishop and a knight
		const bool endgame = (b.isWhiteTurn() ? scorePieceBlack : scorePieceWhite) <= 20000 + 6 * 100 + 333 + 305;
		constexpr Int factor = 1;
		if (endgame)
		{
			finalScore += (b.isWhiteTurn() ? 1 : -1) * (10 * -distanceKings(b));
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
