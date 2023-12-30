#pragma once

#include "search.h"

class SearchRandom : virtual public Search
{
public:
	SearchRandom(const Search::LimitsType &limits) : Search(limits) {}
	SearchRandom(const SearchRandom &s) : Search(s.Limits) {}
	~SearchRandom() {}

	cMove nextMove(const BoardParser &b, const Evaluate &) override
	{
		std::vector<cMove> moveList = std::vector<cMove>();
		for (UInt tile = 0; tile < BOARD_SIZE2; ++tile)
		{
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
			moveList.insert(moveList.end(), subMoveList.begin(), subMoveList.end());
		}
		if (moveList.empty())
		{
			err("Cannot move after checkmate.");
			return cMove();
		}
		// Verify not in check
		BoardParser b2;
		cMove move;
		do
		{
			b2 = BoardParser(b);

			UInt idx = UInt(double(std::rand()) / double(RAND_MAX) * double(moveList.size()));
			move = moveList[idx];
			b2.movePiece(move);
			moveList.erase(moveList.begin() + idx);
		} while (b2.inCheck(!b2.isWhiteTurn()) && !moveList.empty());
		return move;
	}
};
