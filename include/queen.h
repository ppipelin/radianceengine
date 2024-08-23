#pragma once

#include "piece.h"

class Queen : public Piece
{
public:
	Queen(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Queen(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Queen() override = default;
	Queen(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::QUEEN; }

	void canAttack(Bitboard &bb) const;

	void canMove(const Board &b, std::vector<cMove> &v, bool legal, bool capture) const;

	bool exists() const override;

	std::string str() const override;
};
