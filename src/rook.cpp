#include "rook.h"

void Rook::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 14);
	constexpr std::array<Int, 4> directions{ 1, -1, 8, -8 };
	for (const Int direction : directions)
	{
		sliding(b, direction, v);
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
