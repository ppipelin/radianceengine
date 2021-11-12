#pragma once

#include "include.h"
#include "board.h"

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
	Piece(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(BOARD_SIZE *b + (BOARD_SIZE - a), isWhite, neverMoved) {}

	/**
		* @brief Construct a new Piece object with one coordinate.
		*
		* @param tile is the tile number where the piece here, 0 being bottom left.
		* @param isWhite
		* @param neverMoved
		*/
	Piece(UInt tile, bool isWhite = true, bool neverMoved = true) : m_tile(tile), m_isWhite(isWhite), m_neverMoved(neverMoved) {}
	Piece() {}
	~Piece() {}

	// Mutators
	UInt &tile() { return m_tile; }

	const UInt &tile() const { return m_tile; }

	/**
		* @brief Returns a vector of the tiles where Piece can move.
		*
		* @param b current board.
		* @param v	vector of tiles where Piece can move.
		*/
	virtual void canMove(const Board &, std::vector<UInt> &) const
	{}

	void sliding(const Board &b, Int direction, std::vector<UInt> &v) const;

	const bool isWhite() const;

	virtual bool exists() const;

	/**
		* @brief string output corresponding to the current Piece
		*
		* @return const std::string
		*/
	virtual std::string str() const;
};
