#pragma once

#include <array>

#include "board.h"
#include "include.h"
#include "piece.h"

class Knight : public Piece
{
public:
	Knight(UInt a, UInt b, bool isWhite = true, bool neverMoved = true) : Piece(a, b, isWhite, neverMoved) {}
	Knight(UInt a, bool isWhite = true, bool neverMoved = true) : Piece(a, isWhite, neverMoved) {}
	~Knight() override = default;
	Knight(const Piece &p)
	{
		m_tile = p.tile();
		m_isWhite = p.isWhite();
		m_neverMoved = p.neverMoved();
	}

	PieceType value() const override { return PieceType::KNIGHT; }

	static constexpr Bitboard filterMoves(UInt tile, bool = false)
	{
		Bitboard b = 0ULL;

		const std::array<std::array<Int, 2>, BOARD_SIZE> knightMoves = { {
			{-2, -1}, {-2, 1}, // 2 bottom
			{-1, -2}, {-1, 2}, // 2 mid-bottom
			{1, -2}, {1, 2}, // 2 mid-top
			{2, -1}, {2, 1} // 2 top
		} };

		for (auto &move : knightMoves)
		{
			Int newRow = tile / BOARD_SIZE + move[0];
			Int newCol = tile % BOARD_SIZE + move[1];
			if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE)
			{
				UInt newTile = newRow * BOARD_SIZE + newCol;
				b |= Bitboards::tileToBB(newTile);
			}
		}

		return b;

	}

	void canMove(const Board &b, std::vector<cMove> &v) const;

	bool exists() const override;

	std::string str() const override;
};
