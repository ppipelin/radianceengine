#include "queen.h"

void Queen::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 27);
	constexpr std::array<Int, 8> directions{ 1, -1, 8, -8, 9, -9, 7, -7 };
	for (const Int direction : directions)
	{
		sliding(b, direction, v);
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
