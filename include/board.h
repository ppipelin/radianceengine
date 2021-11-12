#pragma once

#include "include.h"

class Piece;

/**
	* @brief Board	class
	* @details	This class is used to represent the board.
	*  It contains the pieces in `m_board`. Which is a 64 vector of all the cases.
	*  Index 0 is bottom left corner (A1), index 63 is top right corner (H8).
	*  It also contains the methods to check if a king is in check.
	*
	*/
class Board
{
private:
	// TODO: perhaps set to const Piece
	std::array<Piece *, BOARD_SIZE2> m_board;
	std::vector<UInt> m_whitePos;
	std::vector<UInt> m_blackPos;
public:
	Board()
	{
		m_board = std::array<Piece *, BOARD_SIZE2>();
		m_whitePos.reserve(BOARD_SIZE * 2);
		m_blackPos.reserve(BOARD_SIZE * 2);
	}

	~Board()
	{}

	// Mutators
	std::array<Piece *, BOARD_SIZE2> &board() { return m_board; }

	const std::array<Piece *, BOARD_SIZE2> &board() const { return m_board; }

	Piece &operator[](std::size_t idx) { return *board()[idx]; }

	const Piece &operator[](std::size_t idx) const { return *board()[idx]; }

	std::vector<UInt> &whitePos() { return m_whitePos; }

	const std::vector<UInt> &whitePos() const { return m_whitePos; }

	std::vector<UInt> &blackPos() { return m_blackPos; }

	const std::vector<UInt> &blackPos() const { return m_blackPos; }

	bool sameLine(UInt a, UInt b) const
	{
		return floor(a / BOARD_SIZE) == floor(b / BOARD_SIZE);
	}

	bool sameColumn(UInt a, UInt b) const
	{
		return a % BOARD_SIZE == b % BOARD_SIZE;
	}

	static bool leftCol(UInt tile)
	{
		return tile % BOARD_SIZE == 0; // is 0 % smth always true ?
	}

	static bool rightCol(UInt tile)
	{
		if (tile > BOARD_SIZE2 - 1)
			err("Going beyond board.");
		return (tile + 1) % (BOARD_SIZE) == 0;
	}

	static bool botRow(UInt tile)
	{
		return tile < BOARD_SIZE;
	}

	static bool topRow(UInt tile)
	{
		if (tile > BOARD_SIZE2 - 1)
			err("Going beyond board.");
		return tile >= BOARD_SIZE * (BOARD_SIZE - 1);
	}

	static UInt row(UInt tile)
	{
		return tile / BOARD_SIZE;
	}

	static UInt column(UInt tile)
	{
		return tile % BOARD_SIZE;
	}

	static UInt toTiles(std::string s)
	{
		if (s.length() != 2) return 0;
		UInt letter = s[0] - 'a';
		try
		{
			UInt number = s[1] - '0';
			if (letter > 8 || number > 8)
			{
				err("Invalid tile.");
				return 0;
			}
			return letter + (number - 1) * BOARD_SIZE;
		}
		catch (const std::exception &) { return 0; }
	}
};
