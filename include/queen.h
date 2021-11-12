#pragma once

#include "piece.h"

class Queen : public Piece
{
public:
	Queen(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Queen(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Queen() {}
	void canMove(const Board &b, std::vector<UInt> &v) const;

	bool exists() const override;

	std::string str() const override;
};
