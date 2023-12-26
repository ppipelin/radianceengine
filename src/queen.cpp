#include "queen.h"

void Queen::canMove(const Board &b, std::vector<cMove> &v) const
{
	std::vector<Int> directions{ 1, -1, 8, -8, 9, -9, 7, -7 }; // constexpr in c++20 still limited
	for (Int direction : directions)
	{
		std::vector<cMove> w;
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
