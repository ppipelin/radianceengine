#include <string>
#include <vector>

#include "board.h"
#include "cMove.h"
#include "include.h"
#include "piece.h"

void Piece::sliding(const Board &b, Int direction, std::vector<cMove> &v) const
{
	if (b[m_tile] == nullptr)
	{
		err("we should never make a non existentpiece sliding, this piece is supposed to be nullptr");
		return;
	}

	// Direction +
	if (direction > 0)
	{
		// first condition if going vertically second if going horizontally
		for (UInt i = m_tile; i < BOARD_SIZE2; i += direction)
		{
			if (i != m_tile)
			{
				if (b[i] != nullptr)
				{
					if (b[i]->isWhite() != b[m_tile]->isWhite())
					{
						v.push_back(cMove(m_tile, i, 4));
					}
					// break even if there is a pin
					break;
				}
				v.push_back(cMove(m_tile, i));
			}
			// Check right and left column
			if ((Board::rightCol(i) && (direction == BOARD_SIZE + 1 || direction == 1)) || (Board::leftCol(i) && direction == BOARD_SIZE - 1))
			{
				break;
			}
		}
	}
	else
	{
		for (Int i = m_tile; Int(i) >= 0; i += direction)
		{
			if (UInt(i) != m_tile)
			{

				if (b[i] != nullptr)
				{
					if (b[i]->isWhite() != b[m_tile]->isWhite())
					{
						v.push_back(cMove(m_tile, i, 4));
					}
					// break even if there is a pin
					break;
				}
				v.push_back(cMove(m_tile, i));
			}
			// Check right and left column
			if ((Board::rightCol(i) && direction == -Int(BOARD_SIZE) + 1) || (Board::leftCol(i) && (direction == -Int(BOARD_SIZE) - 1 || direction == -1)))
			{
				break;
			}
		}
	}
}

void Piece::slidingBB(const UInt tile, const Bitboard &blockers, Int direction, Bitboard &bb)
{
	// Direction +
	if (direction > 0)
	{
		// first condition if going vertically second if going horizontally
		for (UInt i = tile; i < BOARD_SIZE2; i += direction)
		{
			if (i != tile)
			{
				bb |= Bitboards::tileToBB(i);
				if (Bitboards::tileToBB(i) & blockers)
				{
					break;
				}
			}
			// Check right and left column
			if ((Board::rightCol(i) && (direction == BOARD_SIZE + 1 || direction == 1)) || (Board::leftCol(i) && direction == BOARD_SIZE - 1))
			{
				break;
			}
		}
	}
	else
	{
		for (Int i = tile; Int(i) >= 0; i += direction)
		{
			if (UInt(i) != tile)
			{
				bb |= Bitboards::tileToBB(i);
				if (Bitboards::tileToBB(i) & blockers)
				{
					break;
				}
			}
			// Check right and left column
			if ((Board::rightCol(i) && direction == -Int(BOARD_SIZE) + 1) || (Board::leftCol(i) && (direction == -Int(BOARD_SIZE) - 1 || direction == -1)))
			{
				break;
			}
		}
	}
}

const bool Piece::isWhite() const
{
	return m_isWhite;
}

const bool Piece::neverMoved() const
{
	return m_neverMoved;
}

bool Piece::exists() const
{
	return false;
}

std::string Piece::str() const
{
	return " ";
}
