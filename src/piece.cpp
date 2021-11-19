#include "piece.h"

void Piece::sliding(const Board &b, Int direction, std::vector<UInt> &v) const
{
	if (b.board()[m_tile] == nullptr)
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
				if (b.board()[i] != nullptr)
				{
					if (b[i].isWhite() != b[m_tile].isWhite())
					{
						v.push_back(i);
					}
					// break even if there is a pin
					break;
				}
				v.push_back(i);
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

				if (b.board()[i] != nullptr)
				{
					if (b[i].isWhite() != b[m_tile].isWhite())
					{
						v.push_back(UInt(i));
					}
					// break even if there is a pin
					break;
				}
				v.push_back(UInt(i));
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
