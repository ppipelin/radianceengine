#include <array>
#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "knight.h"

void Knight::canMove(const Board &b, std::vector<cMove> &v) const
{
	v.reserve(v.size() + 8);

	const std::array<std::array<Int, 2>, 8> knightMoves = { {
		{-2, -1}, {-2, 1}, // 2 bottom
		{-1, -2}, {-1, 2}, // 2 mid-bottom
		{1, -2}, {1, 2}, // 2 mid-top
		{2, -1}, {2, 1} // 2 top
	} };

	for (auto &move : knightMoves)
	{
		Int newRow = m_tile / BOARD_SIZE + move[0];
		Int newCol = m_tile % BOARD_SIZE + move[1];
		if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE)
		{
			UInt newTile = newRow * BOARD_SIZE + newCol;
			if (b[newTile] == nullptr || b[newTile]->isWhite() != isWhite())
			{
				v.push_back(cMove(m_tile, newTile, b[newTile] == nullptr ? 0 : 4));
			}
		}
	}
}

bool Knight::exists() const
{
	return true;
}

std::string Knight::str() const
{
	return m_isWhite ? "N" : "n";
}
