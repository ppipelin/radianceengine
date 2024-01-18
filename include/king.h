#pragma once

#include "piece.h"

class King : public Piece
{
public:
	King(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	King(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~King() {}
	King(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	UInt value() const override { return 6; }

	void canMove(const Board &b, std::vector<cMove> &v) const;

	bool exists() const override;

	std::string str() const override;
};
