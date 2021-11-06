#pragma once

#include "piece.h"

class Pawn : public Piece
{
public:
	Pawn(UInt a, UInt b, bool isWhite, bool neverMoved) : Piece(a, b, isWhite, neverMoved) {}
	Pawn(UInt a, bool isWhite, bool neverMoved) : Piece(a, isWhite, neverMoved) {}
	~Pawn() {}

	void canMove(const Board &b, std::vector<UInt> &v) const;

	bool exists() const override;

	std::string str() const override;
};
