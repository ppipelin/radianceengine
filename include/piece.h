#pragma once

#include "include.h"

/**
	* @brief Piece class.
	* @details This class is used to represent a piece on the board.
	* 	It contains the piece's color, if it has moved, and position.
	* 	It also contains functions to move the piece, and to check if the piece is in check.
	*
	*/
class Piece
{
protected:
	UInt m_tile;
	bool m_isWhite;
	bool m_neverMoved;

public:
	/**
		* @brief Construct a new Piece object with two coordinates.
		*
		* @param a column number, letter, bottom to top.
		* @param b row number, number, left to right.
		* @param isWhite
		* @param neverMoved
		*/
	Piece(UInt a, UInt b, bool isWhite = false, bool neverMoved = false) : Piece(BOARD_SIZE *b + (BOARD_SIZE - a), isWhite, neverMoved) {}

	/**
		* @brief Construct a new Piece object with one coordinate.
		*
		* @param tile is the tile number where the piece here, 0 being bottom left.
		* @param isWhite
		* @param neverMoved
		*/
	Piece(UInt tile, bool isWhite = false, bool neverMoved = false) : m_tile(tile), m_isWhite(isWhite), m_neverMoved(neverMoved) {}
	Piece() {}
	~Piece() {}

	/**
		* @brief Returns a vector of the tiles where Piece can move.
		*
		* @param b current board.
		* @param v	vector of tiles where Piece can move.
		*/
	virtual void canMove(const Board &, std::vector<UInt> &) const
	{}

	void sliding(const Board &b, Int direction, std::vector<UInt> &v) const
	{
		// Direction +
		if (direction > 0)
		{
			// first condition if going vertically second if going horizontally
			for (UInt i = m_tile; i < BOARD_SIZE2; i += direction)
			{
				if (i != m_tile)
				{
					if (b[i].exists())
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

					if (b[i].exists())
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

	const bool isWhite() const
	{
		return m_isWhite;
	}

	virtual bool exists() const
	{
		return false;
	}

	/**
		* @brief string output corresponding to the current Piece
		*
		* @return const std::string
		*/
	virtual std::string str() const
	{
		return " ";
	}
};
