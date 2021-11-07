#include "rook.h"

void Rook::canMove(const Board &b, std::vector<UInt> &v) const
{
	std::vector<Int> directions = { 1, -1, 8, -8 };
	for (Int direction : directions)
	{
		std::vector<UInt> w;
		sliding(b, direction, w);
		v.insert(v.end(), w.begin(), w.end());
	}
}

bool Rook::exists() const
{
	return true;
}

std::string Rook::str() const
{
	return m_isWhite ? "R" : "r";
}
