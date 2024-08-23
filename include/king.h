#pragma once

#include "piece.h"

class King : public Piece
{
public:
	King(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	King(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~King() override = default;
	King(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::KING; }

	void canAttack(Bitboard &bb) const;

	void canMove(const Board &b, std::vector<cMove> &v, bool legal, bool capture) const;

	bool exists() const override;

	std::string str() const override;
};
