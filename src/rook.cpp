#include "rook.h"

void Rook::canMove(const Board &b, std::vector<cMove> &v) const
{
	std::vector<Int> directions{ 1, -1, 8, -8 }; // constexpr in c++20 still limited
	for (Int direction : directions)
	{
		std::vector<cMove> w;
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
