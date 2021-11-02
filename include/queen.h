#pragma once

#include "piece.h"

class Queen : public Piece
{
public:
	Queen(UInt a, UInt b, bool isWhite, bool neverMoved) : Piece(a, b, isWhite, neverMoved) {}
	Queen(UInt a, bool isWhite, bool neverMoved) : Piece(a, isWhite, neverMoved) {}
	~Queen() {}
	void canMove(const Board &b, std::vector<UInt> &v) const
	{
		std::vector<Int> directions = { 1, -1, 8, -8, 9, -9, 7, -7 };
		for (Int direction : directions)
		{
			std::vector<UInt> w;
			sliding(b, direction, w);
			v.insert(v.end(), w.begin(), w.end());
		}
	}

	bool exists() const override
	{
		return true;
	}

	std::string str() const override
	{
		return m_isWhite ? "Q" : "q";
	}
};
