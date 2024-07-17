#pragma once

#include "search.h"

class SearchRandom : virtual public Search
{
public:
	SearchRandom(const Search::LimitsType &limits, bool *g_stop) : Search(limits, g_stop) {}
	SearchRandom(const SearchRandom &s) : Search(s.Limits, s.g_stop) {}
	~SearchRandom() {}

	cMove nextMove(const BoardParser &b, const Evaluate &) override
	{
		const std::lock_guard<std::mutex> lock(mtx);
		std::vector<cMove> moveList;
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
			piece->canMove(*b.boardParsed(), moveList);
		}
		if (moveList.empty())
		{
			err("Cannot move after checkmate.");
			return cMove();
		}
		else if (moveList.size() == 1)
		{
			return moveList[1];
		}
		// Verify not in check
		BoardParser b2;
		cMove move;
		do
		{
			BoardParser::State s;
			b2 = BoardParser(b);

			UInt idx = UInt(double(std::rand()) / double(RAND_MAX) * double(moveList.size()));
			move = moveList[idx];
			b2.movePiece(move, s);
			moveList.erase(moveList.begin() + idx);
		} while (b2.inCheck(!b2.isWhiteTurn()) && !moveList.empty());
		return move;
	}
};
