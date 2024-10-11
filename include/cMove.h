#pragma once

#include "include.h"

/**
	* @brief chess move described like in https://www.chessprogramming.org/Encoding_Moves
	*
	*/
class cMove
{
public:
	cMove() { m_move = 0; }
	cMove(const cMove &c) { m_move = c.m_move; }

	cMove(UInt from, UInt to, UInt flags = 0)
	{
		m_move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
	}

	cMove operator=(cMove c) { m_move = c.m_move; return c; }

	UInt getTo() const { return m_move & 0x3f; }
	UInt getFrom() const { return (m_move >> 6) & 0x3f; }
	UInt getFlags() const { return (m_move >> 12) & 0x0f; }

	void setTo(UInt to) { m_move &= ~0x3f; m_move |= to & 0x3f; }
	void setFrom(UInt from) { m_move &= ~0xfc0; m_move |= (from & 0x3f) << 6; }
	void setFlags(UInt flags) { m_move &= ~0xF000;	m_move |= (flags & 0x0f) << 12; }

	bool isCapture() const { return bool(getFlags() & 0x4); }
	bool isCastle() const { return bool((getFlags() ^ 0x2) <= 1); }
	bool isPromotion() const { return getFlags() >> 3; }

	UInt getButterflyIndex() const { return m_move & 0x0fff; }
	bool operator==(cMove a) const { return (m_move & 0xffff) == (a.m_move & 0xffff); }
	bool operator==(UInt a) const { return m_move == a; }
	bool operator!=(cMove a) const { return (m_move & 0xffff) != (a.m_move & 0xffff); }

	// protected:
	UInt m_move; // Has to be 16 bits
};
