#pragma once

#include "piece.h"

class Rook : public Piece
{
public:
	Rook(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Rook(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Rook() {}
	Rook(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	UInt value() const override { return 4; }

	void canMove(const Board &b, std::vector<cMove> &v) const;

	bool exists() const override;

	std::string str() const override;
};
