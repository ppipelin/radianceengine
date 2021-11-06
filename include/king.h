#pragma once

#include "piece.h"

class King : public Piece
{
public:
	King(UInt a, UInt b, bool isWhite, bool neverMoved) : Piece(a, b, isWhite, neverMoved) {}
	King(UInt a, bool isWhite, bool neverMoved) : Piece(a, isWhite, neverMoved) {}
	~King() {}

	void canMove(const Board &b, std::vector<UInt> &v) const;

	bool exists() const override;

	std::string str() const override;
};
