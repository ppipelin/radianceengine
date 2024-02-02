#include "bishop.h"

void Bishop::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 13);
	constexpr std::array<Int, 4> directions{ -9, -7, 7, 9 };
	for (const Int direction : directions)
	{
		sliding(b, direction, v);
	}
}

bool Bishop::exists() const
{
	return true;
}

std::string Bishop::str() const
{
	return m_isWhite ? "B" : "b";
}
