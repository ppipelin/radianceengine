#pragma once

#include "piece.h"

class Bishop : public Piece
{
public:
	Bishop(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Bishop(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Bishop() {}

	void canMove(const Board &b, std::vector<UInt> &v) const;

	bool exists() const override;

	std::string str() const override;
};
