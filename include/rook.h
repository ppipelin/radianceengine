#pragma once

#include "piece.h"

class Rook : public Piece
{
public:
	Rook(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Rook(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Rook() {}

	void canMove(const Board &b, std::vector<UInt> &v) const;

	bool exists() const override;

	std::string str() const override;
};
