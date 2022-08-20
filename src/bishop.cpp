#include "bishop.h"

void Bishop::canMove(const Board &b, std::vector<cMove> &v) const
{
	std::vector<Int> directions = { 9, -9, 7, -7 };
	for (Int direction : directions)
	{
		std::vector<cMove> w;
		sliding(b, direction, w);
		v.insert(v.end(), w.begin(), w.end());
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
