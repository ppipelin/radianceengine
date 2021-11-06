#include "queen.h"

void Queen::canMove(const Board &b, std::vector<UInt> &v) const
{
	std::vector<Int> directions = { 1, -1, 8, -8, 9, -9, 7, -7 };
	for (Int direction : directions)
	{
		std::vector<UInt> w;
		sliding(b, direction, w);
		v.insert(v.end(), w.begin(), w.end());
	}
}

bool Queen::exists() const
{
	return true;
}

std::string Queen::str() const
{
	return m_isWhite ? "Q" : "q";
}
