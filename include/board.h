#pragma once

#include "include.h"
#include <algorithm>

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
	std::array<Piece *, BOARD_SIZE2> m_board;
	std::vector<UInt> m_whitePos;
	std::vector<UInt> m_blackPos;
	// Column if the en passant can be done next turn, happens when a pawn double forward at row 1/6, -1 otherwise.
	Int m_enPassant = -1;

public:
	bool m_castleAvailableQueenWhite = true, m_castleAvailableKingWhite = true, m_castleAvailableQueenBlack = true, m_castleAvailableKingBlack = true;

	Board()
	{
		// std::array are auto set to nullptr but des not seem to be the case
		// m_board = std::array<Piece *, BOARD_SIZE2>();
		for (auto &p : m_board)
		{
			p = nullptr;
		}
		m_whitePos.reserve(BOARD_SIZE * 2);
		m_blackPos.reserve(BOARD_SIZE * 2);
		m_enPassant = -1;
	}

	~Board()
	{}

	// Mutators
	std::array<Piece *, BOARD_SIZE2> &board() { return m_board; }

	const std::array<Piece *, BOARD_SIZE2> &board() const { return m_board; }

	void enPassant(Int i) { m_enPassant = i; }

	const Int enPassant() const { return m_enPassant; }

	Piece *operator[](std::size_t idx) { return board()[idx]; }

	const Piece *operator[](std::size_t idx) const { return board()[idx]; }

	bool operator==(const Board &b) const
	{
		auto wp = whitePos();
		auto wp2 = b.whitePos();
		auto bp = blackPos();
		auto bp2 = b.blackPos();
		std::sort(wp.begin(), wp.end());
		std::sort(wp2.begin(), wp2.end());
		std::sort(bp.begin(), bp.end());
		std::sort(bp2.begin(), bp2.end());
		if (wp != wp2 || bp != bp2)
			return false;

		if (m_castleAvailableQueenWhite != b.m_castleAvailableQueenWhite || m_castleAvailableKingWhite != b.m_castleAvailableKingWhite ||
			m_castleAvailableQueenBlack != b.m_castleAvailableQueenBlack || m_castleAvailableKingBlack != b.m_castleAvailableKingBlack)
			return false;

		if (enPassant() != b.enPassant())
			return false;
		return true;
	}

	std::vector<UInt> &whitePos() { return m_whitePos; }

	const std::vector<UInt> &whitePos() const { return m_whitePos; }

	std::vector<UInt> &blackPos() { return m_blackPos; }

	const std::vector<UInt> &blackPos() const { return m_blackPos; }

	static bool sameLine(UInt a, UInt b)
	{
		return floor(a / BOARD_SIZE) == floor(b / BOARD_SIZE);
	}

	static bool sameColumn(UInt a, UInt b)
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

	static UInt toTiles(const std::string &s)
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

	static std::string toString(const UInt tile)
	{
		return std::string(1, char('a' + tile % BOARD_SIZE)) + std::string(1, char('1' + (tile / BOARD_SIZE)));
	}
};
