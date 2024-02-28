#pragma once

#include "include.h"

#include "piece.h"
#include "pawn.h"
#include "king.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"

#include "board.h"
#include "cMove.h"

#include <sstream>
#include <algorithm>
#include <unordered_map>

namespace {
	namespace Zobrist {
		// KQRBNPkqrbnp
		Key psq[12][BOARD_SIZE2];
		// KQkq
		Key enPassant[BOARD_SIZE];
		Key castling[4];
		Key side;
	}
}

/**
	* @brief This class is used to parse the board and is aware of the Piece's type.
	* @details To be able to know the if castle is available, it contains the movePiece() function.
	*/
class BoardParser
{
private:
	Board *m_board;
	bool m_isWhiteTurn;
	UInt m_whiteKing = 4;
	UInt m_blackKing = 60;
	const std::string m_starting = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

public:
	Key m_materialKey = 0;

	BoardParser()
	{
		m_board = new Board();
		fillBoard(m_starting);
		m_isWhiteTurn = true;

		PRNG rng(1070372);

		for (UInt pc = 0; pc < 12; ++pc)
			for (UInt tile = 0; tile < BOARD_SIZE2; ++tile)
				Zobrist::psq[pc][tile] = rng.rand<Key>();

		for (UInt col = 0; col <= BOARD_SIZE; ++col)
			Zobrist::enPassant[col] = rng.rand<Key>();

		for (UInt cr = 0; cr < 4; ++cr)
			Zobrist::castling[cr] = rng.rand<Key>();

		Zobrist::side = rng.rand<Key>();
	}

	~BoardParser()
	{
		for (auto &i : m_board->board())
		{
			if (i != nullptr)
			{
				delete i;
				i = nullptr;
			}
		}
		delete m_board;
	}

	BoardParser(const BoardParser &b)
	{
		// Guard self assignment
		if (this == &b)
			return;

		m_board = new Board();
		m_isWhiteTurn = b.isWhiteTurn();
		for (UInt i = 0; i < BOARD_SIZE2; ++i)
		{
			const Piece *p = b.boardParsed()->board()[i];
			if (p == nullptr)
				continue;
			if (typeid(*p) == typeid(Piece))
				m_board->board()[i] = new Piece(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(King))
				m_board->board()[i] = new King(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Queen))
				m_board->board()[i] = new Queen(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Rook))
				m_board->board()[i] = new Rook(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Bishop))
				m_board->board()[i] = new Bishop(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Knight))
				m_board->board()[i] = new Knight(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Pawn))
				m_board->board()[i] = new Pawn(*(*b.boardParsed())[i]);
		}
		m_board->whitePos() = b.boardParsed()->whitePos();
		m_board->blackPos() = b.boardParsed()->blackPos();
		whiteKing(b.whiteKing());
		blackKing(b.blackKing());
		m_materialKey = b.m_materialKey;

		m_board->m_castleAvailableQueenWhite = b.boardParsed()->m_castleAvailableQueenWhite;
		m_board->m_castleAvailableKingWhite = b.boardParsed()->m_castleAvailableKingWhite;
		m_board->m_castleAvailableQueenBlack = b.boardParsed()->m_castleAvailableQueenBlack;
		m_board->m_castleAvailableKingBlack = b.boardParsed()->m_castleAvailableKingBlack;

		m_board->enPassant(b.boardParsed()->enPassant());
	}

	BoardParser &operator=(const BoardParser &b)
	{
		// Guard self assignment
		if (this == &b)
			return *this;

		// Delete previous data before overriding, keep m_board
		for (auto &i : m_board->board())
		{
			if (i != nullptr)
			{
				delete i;
				i = nullptr;
			}
		}
		m_isWhiteTurn = b.isWhiteTurn();
		for (UInt i = 0; i < BOARD_SIZE2; ++i)
		{
			Piece *p = b.boardParsed()->board()[i];
			if (p == nullptr)
				continue;
			if (typeid(*p) == typeid(Piece))
				m_board->board()[i] = new Piece(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(King))
				m_board->board()[i] = new King(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Queen))
				m_board->board()[i] = new Queen(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Rook))
				m_board->board()[i] = new Rook(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Bishop))
				m_board->board()[i] = new Bishop(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Knight))
				m_board->board()[i] = new Knight(*(*b.boardParsed())[i]);
			else if (typeid(*p) == typeid(Pawn))
				m_board->board()[i] = new Pawn(*(*b.boardParsed())[i]);
		}
		m_board->whitePos() = b.boardParsed()->whitePos();
		m_board->blackPos() = b.boardParsed()->blackPos();
		whiteKing(b.whiteKing());
		blackKing(b.blackKing());
		m_materialKey = b.m_materialKey;

		m_board->m_castleAvailableQueenWhite = b.boardParsed()->m_castleAvailableQueenWhite;
		m_board->m_castleAvailableKingWhite = b.boardParsed()->m_castleAvailableKingWhite;
		m_board->m_castleAvailableQueenBlack = b.boardParsed()->m_castleAvailableQueenBlack;
		m_board->m_castleAvailableKingBlack = b.boardParsed()->m_castleAvailableKingBlack;
		return *this;
	}

	bool operator==(const BoardParser &b) const
	{
		if (this == &b)
			return true;

		if (m_isWhiteTurn != b.isWhiteTurn())
			return false;
		for (UInt i = 0; i < BOARD_SIZE2; ++i)
		{
			Piece *p = b.boardParsed()->board()[i];
			Piece *p_this = m_board->board()[i];
			if (p == nullptr && p_this == nullptr)
				continue;
			// Shouldn't be any nullptr now
			if (p == nullptr || p_this == nullptr)
				return false;
			if (typeid(*p) != typeid(*p_this))
				return false;
		}

		if (whiteKing() != b.whiteKing() || blackKing() != b.blackKing())
			return false;

		if (m_materialKey != b.m_materialKey)
			return false;

		return *boardParsed() == *b.boardParsed();
	}

	struct State
	{
		State(const BoardParser &b)
		{
			castleInfo = (b.boardParsed()->m_castleAvailableQueenWhite << 3) | (b.boardParsed()->m_castleAvailableKingWhite << 2) | (b.boardParsed()->m_castleAvailableQueenBlack << 1) | UInt(b.boardParsed()->m_castleAvailableKingBlack);
			enPassant = b.boardParsed()->enPassant();
			lastCapturedPiece = nullptr;
		}
		State() = default;

		UInt castleInfo = 0b1111;
		Int enPassant = -1;
		Piece *lastCapturedPiece = nullptr;
	};

	// Mutators
	Board *boardParsed() { return m_board; }

	const Board *boardParsed() const { return m_board; }

	const bool isWhiteTurn() const { return m_isWhiteTurn; }

	void whiteKing(const UInt whiteKing) { m_whiteKing = whiteKing; }

	const UInt whiteKing() const { return m_whiteKing; }

	void blackKing(const UInt blackKing) { m_blackKing = blackKing; }

	const UInt blackKing() const { return m_blackKing; }

	/**
		* @brief
		*
		* @param move : moving flags as depicted in https://www.chessprogramming.org/Encoding_Moves#From-To_Based
		* @return true : move successful
		* @return false : move illegal
		*/
	bool movePiece(cMove const &move, Piece **lastCapturedPiece = nullptr)
	{
		UInt to = move.getTo();
		UInt from = move.getFrom();
		UInt flags = move.getFlags();
		Piece *fromPiece = m_board->board()[from];
		Piece *toPiece = m_board->board()[to];

		if (fromPiece == nullptr)
		{
			err("moving a nullptr");
			return false;
		}

		// Remove last enPassant
		if (m_board->enPassant() != -1)
		{
			m_materialKey ^= Zobrist::enPassant[Board::column(m_board->enPassant())];
			m_board->enPassant(-1);
		}

		// Disable castle if king/rook is moved
		if (typeid(*fromPiece) == typeid(King))
		{
			if (fromPiece->isWhite())
			{
				whiteKing(to);
				if (boardParsed()->m_castleAvailableKingWhite)
					m_materialKey ^= Zobrist::castling[0];
				if (boardParsed()->m_castleAvailableQueenWhite)
					m_materialKey ^= Zobrist::castling[1];
				boardParsed()->m_castleAvailableKingWhite = false;
				boardParsed()->m_castleAvailableQueenWhite = false;
			}
			else
			{
				blackKing(to);
				if (boardParsed()->m_castleAvailableKingBlack)
					m_materialKey ^= Zobrist::castling[2];
				if (boardParsed()->m_castleAvailableQueenBlack)
					m_materialKey ^= Zobrist::castling[3];
				boardParsed()->m_castleAvailableKingBlack = false;
				boardParsed()->m_castleAvailableQueenBlack = false;
			}
		}
		else if (typeid(*fromPiece) == typeid(Rook))
		{
			if (Board::column(from) == 7)
			{
				bool *castle = &(fromPiece->isWhite() ? boardParsed()->m_castleAvailableKingWhite : boardParsed()->m_castleAvailableKingBlack);
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[(fromPiece->isWhite() ? 0 : 2)];
					*castle = false;
				}
			}
			else	if (Board::column(from) == 0)
			{
				bool *castle = &(fromPiece->isWhite() ? boardParsed()->m_castleAvailableQueenWhite : boardParsed()->m_castleAvailableQueenBlack);
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[(fromPiece->isWhite() ? 1 : 3)];
					*castle = false;
				}
			}
		}
		else if (typeid(*fromPiece) == typeid(Pawn))
		{
			// Updates enPassant if possible next turn
			if (fabs(Int(from) - Int(to)) == 16)
			{
				m_board->enPassant(Board::column(to));
				m_materialKey ^= Zobrist::enPassant[Board::column(to)];
			}
			else
			{
				// En passant
				if (!Board::sameColumn(from, to) && m_board->board()[to] == nullptr)
				{
					// Should never be nullptr
					UInt enPassantTile = to + (fromPiece->isWhite() ? -Int(BOARD_SIZE) : BOARD_SIZE);

					// This if handles legacy behavior providing no pointer to lastCapturedPiece
					if (lastCapturedPiece != nullptr)
						*lastCapturedPiece = m_board->board()[enPassantTile];
					else
						lastCapturedPiece = &m_board->board()[enPassantTile];

					// Remove
					m_materialKey ^= Zobrist::psq[((*lastCapturedPiece)->isWhite() ? 0 : 6) + 6 - (*lastCapturedPiece)->value()][enPassantTile];

					m_board->board()[enPassantTile] = nullptr;
					// Remove in color table
					if (fromPiece->isWhite())
					{
						m_board->blackPos().erase(std::find(m_board->blackPos().begin(), m_board->blackPos().end(), enPassantTile));
					}
					else
					{
						m_board->whitePos().erase(std::find(m_board->whitePos().begin(), m_board->whitePos().end(), enPassantTile));
					}
				}

				// Promotion
				if (move.isPromotion())
				{
					// Before delete we store the data we need
					const bool isWhite = fromPiece->isWhite();
					// Remove
					m_materialKey ^= Zobrist::psq[(fromPiece->isWhite() ? 0 : 6) + 6 - fromPiece->value()][from];
					delete m_board->board()[from];
					m_board->board()[from] = nullptr;
					if (((move.m_move >> 12) & 0x3) == 0)
						fromPiece = new Knight(from, isWhite, false);
					else if (((move.m_move >> 12) & 0x3) == 1)
						fromPiece = new Bishop(from, isWhite, false);
					else if (((move.m_move >> 12) & 0x3) == 2)
						fromPiece = new Rook(from, isWhite, false);
					else if (((move.m_move >> 12) & 0x3) == 3)
						fromPiece = new Queen(from, isWhite, false);
					// Add
					m_materialKey ^= Zobrist::psq[(fromPiece->isWhite() ? 0 : 6) + 6 - fromPiece->value()][from];
				}
			}
		}

		if (toPiece != nullptr)
		{
			// This should be the quickest to disable castle when rook is taken
			bool *castle = nullptr;
			if (to == 0)
			{
				castle = &m_board->m_castleAvailableQueenWhite;
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[1];
					*castle = false;
				}
			}
			else if (to == 7)
			{
				castle = &m_board->m_castleAvailableKingWhite;
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[0];
					*castle = false;
				}
			}
			else if (to == 56)
			{
				castle = &m_board->m_castleAvailableQueenBlack;
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[3];
					*castle = false;
				}
			}
			else if (to == 63)
			{
				castle = &m_board->m_castleAvailableKingBlack;
				if (*castle)
				{
					m_materialKey ^= Zobrist::castling[2];
					*castle = false;
				}
			}

			// This if handles legacy behavior providing no pointer to lastCapturedPiece
			if (lastCapturedPiece != nullptr)
				*lastCapturedPiece = m_board->board()[to];

			// Remove
			m_materialKey ^= Zobrist::psq[(toPiece->isWhite() ? 0 : 6) + 6 - toPiece->value()][to];
			m_board->board()[to] = nullptr;
			// Editing color table for captures
			if (fromPiece->isWhite())
			{
				m_board->blackPos().erase(std::find(m_board->blackPos().begin(), m_board->blackPos().end(), to));
			}
			else
			{
				m_board->whitePos().erase(std::find(m_board->whitePos().begin(), m_board->whitePos().end(), to));
			}
		}

		// Remove
		m_materialKey ^= Zobrist::psq[(fromPiece->isWhite() ? 0 : 6) + 6 - fromPiece->value()][from];
		m_board->board()[from] = nullptr;

		// Add
		m_materialKey ^= Zobrist::psq[(fromPiece->isWhite() ? 0 : 6) + 6 - fromPiece->value()][to];
		m_board->board()[to] = fromPiece;
		fromPiece->tile() = to;

		// Editing color table
		// TODO : use std::replace_if ? or std::replace to avoid loop over all vector
		if (fromPiece->isWhite())
		{
			m_board->whitePos().erase(std::find(m_board->whitePos().begin(), m_board->whitePos().end(), from));
			m_board->whitePos().push_back(to);
		}
		else
		{
			m_board->blackPos().erase(std::find(m_board->blackPos().begin(), m_board->blackPos().end(), from));
			m_board->blackPos().push_back(to);
		}
		m_isWhiteTurn = !m_isWhiteTurn;
		m_materialKey ^= Zobrist::side;

		// If castling we move the rook as well
		if (flags == 2)
		{
			movePiece(cMove(from + 3, from + 3 - 2), /*lastCapturedPiece =*/ nullptr);
			// We have moved, we need to set the turn back
			m_isWhiteTurn = !m_isWhiteTurn;
			m_materialKey ^= Zobrist::side;
		}
		else if (flags == 3)
		{
			movePiece(cMove(from - 4, from - 4 + 3), /*lastCapturedPiece =*/ nullptr);
			// We have moved, we need to set the turn back
			m_isWhiteTurn = !m_isWhiteTurn;
			m_materialKey ^= Zobrist::side;
		}

		return true;
	}

	/**
		* @brief
		*
		* @param move : moving flags as depicted in https://www.chessprogramming.org/Encoding_Moves#From-To_Based
		* @return true : move successful
		* @return false : move illegal
		*/
	bool unMovePiece(const cMove &move, const State &s)
	{
		UInt to = move.getTo();
		UInt from = move.getFrom();
		UInt flags = move.getFlags();
		// Piece *fromPiece = m_board->board()[from];
		Piece *toPiece = m_board->board()[to];

		// Add
		m_materialKey ^= Zobrist::psq[(toPiece->isWhite() ? 0 : 6) + 6 - toPiece->value()][from];
		m_board->board()[from] = toPiece;
		toPiece->tile() = from;
		// Remove
		m_materialKey ^= Zobrist::psq[(toPiece->isWhite() ? 0 : 6) + 6 - toPiece->value()][to];
		m_board->board()[to] = nullptr;

		if (!boardParsed()->m_castleAvailableKingWhite && s.castleInfo & 0b0100)
			m_materialKey ^= Zobrist::castling[0];
		if (!boardParsed()->m_castleAvailableQueenWhite && s.castleInfo & 0b1000)
			m_materialKey ^= Zobrist::castling[1];
		if (!boardParsed()->m_castleAvailableKingBlack && s.castleInfo & 0b0001)
			m_materialKey ^= Zobrist::castling[2];
		if (!boardParsed()->m_castleAvailableQueenBlack && s.castleInfo & 0b0010)
			m_materialKey ^= Zobrist::castling[3];
		boardParsed()->m_castleAvailableKingWhite = s.castleInfo & 0b0100;
		boardParsed()->m_castleAvailableQueenWhite = s.castleInfo & 0b1000;
		boardParsed()->m_castleAvailableKingBlack = s.castleInfo & 0b0001;
		boardParsed()->m_castleAvailableQueenBlack = s.castleInfo & 0b0010;
		if (typeid(*toPiece) == typeid(King))
		{
			if (toPiece->isWhite())
				whiteKing(from);
			else
				blackKing(from);
		}

		// Was a promotion
		if (move.isPromotion())
		{
			// temporary have to delete and new but should be fixed for speedup
			// Before delete we store the data we need
			const bool isWhite = toPiece->isWhite();
			// Remove promoted piece back into pawn (already moved back)
			m_materialKey ^= Zobrist::psq[(toPiece->isWhite() ? 0 : 6) + 6 - toPiece->value()][from];
			delete m_board->board()[from];
			m_board->board()[from] = new Pawn(from, isWhite, false);
			toPiece = m_board->board()[from];
			// Add
			m_materialKey ^= Zobrist::psq[(toPiece->isWhite() ? 0 : 6) + 6 - toPiece->value()][from];
		}

		// Remove added enPassant
		if (m_board->enPassant() != -1)
			m_materialKey ^= Zobrist::enPassant[Board::column(m_board->enPassant())];
		if (s.enPassant != -1)
			m_materialKey ^= Zobrist::enPassant[Board::column(s.enPassant)];
		boardParsed()->enPassant(s.enPassant);

		// Editing color table
		// TODO : use std::replace_if ? or std::replace to avoid loop over all vector
		if (toPiece->isWhite())
		{
			m_board->whitePos().erase(std::find(m_board->whitePos().begin(), m_board->whitePos().end(), to));
			m_board->whitePos().push_back(from);
		}
		else
		{
			m_board->blackPos().erase(std::find(m_board->blackPos().begin(), m_board->blackPos().end(), to));
			m_board->blackPos().push_back(from);
		}

		// Updates enPassant if possible next turn
		if (s.lastCapturedPiece != nullptr)
		{
			UInt localTo = to;
			if (s.lastCapturedPiece->tile() != to)
				localTo = s.lastCapturedPiece->isWhite() ? to + 8 : to - 8;

			m_materialKey ^= Zobrist::psq[(s.lastCapturedPiece->isWhite() ? 0 : 6) + 6 - s.lastCapturedPiece->value()][localTo];
			m_board->board()[localTo] = s.lastCapturedPiece;

			// Editing color table for captures
			if (s.lastCapturedPiece->isWhite())
			{
				m_board->whitePos().push_back(localTo);
			}
			else
			{
				m_board->blackPos().push_back(localTo);
			}
		}

		m_isWhiteTurn = !m_isWhiteTurn;
		m_materialKey ^= Zobrist::side;

		// If castling we move the rook as well
		if (flags == 2)
		{
			unMovePiece(cMove(from + 3, from + 3 - 2), s);
			// We have moved, we need to set the turn back
			m_isWhiteTurn = !m_isWhiteTurn;
			m_materialKey ^= Zobrist::side;
		}
		else if (flags == 3)
		{
			unMovePiece(cMove(from - 4, from - 4 + 3), s);
			// We have moved, we need to set the turn back
			m_isWhiteTurn = !m_isWhiteTurn;
			m_materialKey ^= Zobrist::side;
		}

		return true;
	}

	/**
		* @brief Fills the	board with the position fen
		*
		* @param fen representation to fill the board
		* @return true if board was successfully filled.
		* @return false else
		*/
	bool fillBoard(const std::string &fen)
	{
		std::stringstream sstream(fen);
		std::string word;
		std::vector<std::string> words{};
		while (std::getline(sstream, word, ' '))
		{
			words.push_back(word);
		}

		m_materialKey = 0;
		m_board->whitePos().clear();
		m_board->blackPos().clear();
		m_board->whitePos().reserve(BOARD_SIZE * 2);
		m_board->blackPos().reserve(BOARD_SIZE * 2);
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; const auto & c : words[0])
		{
			if (isdigit(c))
			{
				for (Int i = 0; i < atoi(&c); ++i)
				{
					if (m_board->board()[counter] != nullptr)
					{
						delete m_board->board()[counter];
						m_board->board()[counter] = nullptr;
					}
					++counter;
				}
				continue;
			}
			if (c == '/' && Board::column(counter) != 0)
			{
				err("Going further than board numbers.");
				return false;
			}

			if (c != '/' && m_board->board()[counter] != nullptr)
			{
				delete m_board->board()[counter];
				m_board->board()[counter] = nullptr;
			}

			switch (c)
			{
			case 'p':
				m_board->board()[counter] = new Pawn(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[5 + 6][counter];
				break;
			case 'P':
				m_board->board()[counter] = new Pawn(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[5][counter];
				break;
			case 'k':
				m_board->board()[counter] = new King(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[0 + 6][counter];
				blackKing(counter);
				break;
			case 'K':
				m_board->board()[counter] = new King(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[0][counter];
				whiteKing(counter);
				break;
			case 'q':
				m_board->board()[counter] = new Queen(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[1 + 6][counter];
				break;
			case 'Q':
				m_board->board()[counter] = new Queen(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[1][counter];
				break;
			case 'r':
				m_board->board()[counter] = new Rook(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[2 + 6][counter];
				break;
			case 'R':
				m_board->board()[counter] = new Rook(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[2][counter];
				break;
			case 'b':
				m_board->board()[counter] = new Bishop(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[3 + 6][counter];
				break;
			case 'B':
				m_board->board()[counter] = new Bishop(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[3][counter];
				break;
			case 'n':
				m_board->board()[counter] = new Knight(counter, false, true);
				m_board->blackPos().push_back(counter);
				m_materialKey ^= Zobrist::psq[4 + 6][counter];
				break;
			case 'N':
				m_board->board()[counter] = new Knight(counter, true, true);
				m_board->whitePos().push_back(counter);
				m_materialKey ^= Zobrist::psq[4 + 6][counter];
				break;
			case '/':
				counter -= BOARD_SIZE * 2 + 1;
				break;
			}
			++counter;
		}

		if (words.size() > 1 && words[1] == "w")
		{
			m_isWhiteTurn = true;
			m_materialKey ^= Zobrist::side;
		}
		else
		{
			m_isWhiteTurn = false;
		}

		if (words.size() > 3)
		{
			m_board->m_castleAvailableQueenWhite = false;
			m_board->m_castleAvailableKingWhite = false;
			m_board->m_castleAvailableQueenBlack = false;
			m_board->m_castleAvailableKingBlack = false;
			if (words[2] != "-")
			{
				if (std::find(words[2].begin(), words[2].end(), 'Q') != words[2].end())
				{
					m_board->m_castleAvailableQueenWhite = true;
					m_materialKey ^= Zobrist::castling[1];
				}
				if (std::find(words[2].begin(), words[2].end(), 'q') != words[2].end())
				{
					m_board->m_castleAvailableQueenBlack = true;
					m_materialKey ^= Zobrist::castling[3];
				}
				if (std::find(words[2].begin(), words[2].end(), 'K') != words[2].end())
				{
					m_board->m_castleAvailableKingWhite = true;
					m_materialKey ^= Zobrist::castling[0];
				}
				if (std::find(words[2].begin(), words[2].end(), 'k') != words[2].end())
				{
					m_board->m_castleAvailableKingBlack = true;
					m_materialKey ^= Zobrist::castling[2];
				}
			}
		}
		if (words.size() > 4 && words[3] != "-")
		{
			UInt tile = Board::toTiles(words[3]);
			m_board->enPassant(Board::column(tile));
			m_materialKey ^= Zobrist::enPassant[Board::column(tile)];
		}
		else
		{
			m_board->enPassant(-1);
		}
		return true;
	}

	std::string fen(const bool noEnPassant = false) const
	{
		std::string s = "";
		UInt accumulate = 0;
		for (UInt i = 56; i != BOARD_SIZE; ++i)
		{
			if ((!s.empty() || i != 56) && i % 8 == 0)
			{
				i -= 16;
				if (accumulate != 0)
				{
					s += std::to_string(accumulate);
					accumulate = 0;
				}
				s += "/";
			}
			const Piece *p = m_board->board()[i];
			if (p == nullptr)
			{
				++accumulate;
			}
			else
			{
				if (accumulate != 0)
				{
					s += std::to_string(accumulate);
					accumulate = 0;
				}
				if (typeid(*p) == typeid(King))
					s += p->isWhite() ? "K" : "k";
				else if (typeid(*p) == typeid(Queen))
					s += p->isWhite() ? "Q" : "q";
				else if (typeid(*p) == typeid(Rook))
					s += p->isWhite() ? "R" : "r";
				else if (typeid(*p) == typeid(Bishop))
					s += p->isWhite() ? "B" : "b";
				else if (typeid(*p) == typeid(Knight))
					s += p->isWhite() ? "N" : "n";
				else if (typeid(*p) == typeid(Pawn))
					s += p->isWhite() ? "P" : "p";
			}
		}
		if (accumulate != 0)
		{
			s += std::to_string(accumulate);
			accumulate = 0;
		}

		s += isWhiteTurn() ? " w " : " b ";

		std::string caslteStr = "";
		if (m_board->m_castleAvailableKingWhite)
			caslteStr += "K";
		if (m_board->m_castleAvailableQueenWhite)
			caslteStr += "Q";
		if (m_board->m_castleAvailableKingBlack)
			caslteStr += "k";
		if (m_board->m_castleAvailableQueenBlack)
			caslteStr += "q";

		if (caslteStr.empty())
			caslteStr = "-";
		s += caslteStr;

		s += " ";

		Int enPassant = m_board->enPassant();
		s += noEnPassant || enPassant == -1 ? "-" : Board::toString((isWhiteTurn() ? 40 : 16) + enPassant);

		return s;
	}

	bool inCheck(bool isWhite, std::array<cMove, MAX_PLY> vTotal = std::array<cMove, MAX_PLY>(), size_t arraySize = 0) const
	{
		UInt kingPos = isWhite ? whiteKing() : blackKing();
		// Compute all oponents moves
		if (arraySize == 0)
		{
			std::vector<cMove> v;
			for (const auto tile : (!isWhite ? m_board->whitePos() : m_board->blackPos()))
			{
				const Piece *piece = m_board->board()[tile];
				if (piece == nullptr)
				{
					continue;
				}
				piece->canMove(*m_board, v);
			}
			// TODO: parallelize
			return std::find_if(v.begin(), v.end(), [kingPos](const auto &ele) {return ele.getTo() == kingPos;}) != v.end();
		}
		else
		{
			// TODO: parallelize
			return std::find_if(vTotal.begin(), vTotal.begin() + arraySize, [kingPos](const auto &ele) {return ele.getTo() == kingPos;}) != vTotal.begin() + arraySize;
		}

	}

	void displayCout()
	{
		std::cout << "displayCout" << std::endl;
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			const Piece *value = m_board->board()[counter];
			if (Board::column(counter + 1) == 0)
			{
				counter -= BOARD_SIZE * 2;
			}
			if (value == nullptr)
			{
				std::cout << " ";
				continue;
			}
			std::cout << value->str();
		}
	}

	void displayCLI()
	{
		std::string out;
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			const Piece *value = m_board->board()[counter];
			out.append("|");
			if (value == nullptr)
			{
				out.append(" ");
			}
			else
			{
				out.append(value->str());
			}
			if (Board::column(counter + 1) == 0)
			{
				out.append("|\n");
				counter -= BOARD_SIZE * 2;
			}
		}
		out.append("|");
		const Piece *value = m_board->board()[BOARD_SIZE - 1];
		if (value == nullptr)
		{
			out.append(" ");
		}
		else
		{
			out.append(value->str());
		}

		std::cout << out << std::string("|") << std::endl << std::endl;
	}

	/**
		* @brief Display board in CLI by parsing the optimized vectors.
		*
		*/
	void displayCLIWhiteBlack()
	{
		std::string out;
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			out.append("|");
			std::string s;
			auto whiteFind = std::find(m_board->whitePos().begin(), m_board->whitePos().end(), counter);
			auto blackFind = std::find(m_board->blackPos().begin(), m_board->blackPos().end(), counter);
			if (whiteFind != m_board->whitePos().end())
			{
				out.append("O");
			}
			else if (blackFind != m_board->blackPos().end())
			{
				out.append("X");
			}
			else
			{
				out.append(" ");
			}
			if (Board::column(counter + 1) == 0)
			{
				out.append("|\n");
				counter -= BOARD_SIZE * 2;
			}
		}
		out.append("|");
		std::string s;
		auto whiteFind = std::find(m_board->whitePos().begin(), m_board->whitePos().end(), BOARD_SIZE - 1);
		auto blackFind = std::find(m_board->blackPos().begin(), m_board->blackPos().end(), BOARD_SIZE - 1);
		if (whiteFind != m_board->whitePos().end())
		{
			out.append("O");
		}
		else if (blackFind != m_board->blackPos().end())
		{
			out.append("X");
		}
		else
		{
			out.append(" ");
		}
		std::cout << out << std::string("|") << std::endl << std::endl;
	}

	/**
		* @brief Display board in CLI and shows how can a piece move.
		*
		*/
	void displayCLIMove(UInt tile)
	{
		std::string out;
		const Piece *piece = m_board->board()[tile];
		std::vector<cMove> v;
		if (piece != nullptr)
		{
			piece->canMove(*m_board, v);
		}

		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			const Piece *value = m_board->board()[counter];
			out.append("|");
			if (std::find_if(v.begin(), v.end(), [counter](const auto &ele) {return ele.getTo() == counter;}) != v.end())
			{
				out.append("X");
			}
			else if (value == nullptr)
			{
				out.append(" ");
			}
			else
			{
				out.append(value->str());
			}
			if (Board::column(counter + 1) == 0)
			{
				out.append("|\n");
				counter -= BOARD_SIZE * 2;
			}
		}
		out.append("|");
		if (std::find_if(v.begin(), v.end(), [](const auto &ele) {return ele.getTo() == BOARD_SIZE - 1;}) != v.end())
		{
			out.append("X");
		}
		else
		{
			const Piece *value = m_board->board()[BOARD_SIZE - 1];
			if (value == nullptr)
			{
				out.append(" ");
			}
			else
			{
				out.append(value->str());
			}
		}

		std::cout << out << std::string("|") << std::endl << std::endl;
	}
};
