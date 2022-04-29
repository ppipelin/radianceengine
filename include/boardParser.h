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

#include <sstream>

/**
	* @brief This class is used to parse the board and is aware of the Piece's type.
	* @details To be able to know the if castle is available, it contains the movePiece()	function.
	*/
class BoardParser
{
private:
	Board *m_board;
	bool m_isWhiteTurn;
	const std::string m_starting = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

public:
	BoardParser()
	{
		m_board = new Board();
		fillBoard(m_starting);
		m_isWhiteTurn = true;
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
		m_board = new Board();
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

		m_board->m_castleAvailableQueenWhite = b.boardParsed()->m_castleAvailableQueenWhite;
		m_board->m_castleAvailableKingWhite = b.boardParsed()->m_castleAvailableKingWhite;
		m_board->m_castleAvailableQueenBlack = b.boardParsed()->m_castleAvailableQueenBlack;
		m_board->m_castleAvailableKingBlack = b.boardParsed()->m_castleAvailableKingBlack;
	}

	// Mutators
	Board *boardParsed() { return m_board; }

	const Board *boardParsed() const { return m_board; }

	const bool isWhiteTurn() const { return m_isWhiteTurn; }

	void movePiece(UInt	from, UInt to)
	{
		Piece *fromPiece = m_board->board()[from];
		Piece *toPiece = m_board->board()[to];
		if (fromPiece == nullptr)
		{
			err("moving a nullptr");
			return;
		}
		// Disable castle if king/rook is moved
		if (typeid(*fromPiece) == typeid(King))
		{
			if (fromPiece->isWhite())
			{
				boardParsed()->m_castleAvailableKingWhite = false;
				boardParsed()->m_castleAvailableQueenWhite = false;
			}
			else
			{
				boardParsed()->m_castleAvailableKingBlack = false;
				boardParsed()->m_castleAvailableQueenBlack = false;
			}
			// if actually castling
			if (abs(Int(to) - Int(from)) == 2)
			{
				if (fromPiece->isWhite())
				{
					// Queen Castle
					if (to < from)
					{
						movePiece(0, 3);
					}
					else
					{
						movePiece(BOARD_SIZE - 1, BOARD_SIZE - 3);
					}
				}
				else
				{
					// Queen Castle
					if (to < from)
					{
						movePiece(BOARD_SIZE2 - BOARD_SIZE, BOARD_SIZE2 - BOARD_SIZE + 3);
					}
					else
					{
						movePiece(BOARD_SIZE2 - 1, BOARD_SIZE2 - 3);
					}
				}
				// We have moved, we need to set the turn back
				m_isWhiteTurn = !m_isWhiteTurn;
			}
		}
		else if (typeid(*fromPiece) == typeid(Rook))
		{
			// if (fromPiece->isWhite())
			// {
			if (Board::column(from) == 7)
			{
				boardParsed()->m_castleAvailableKingWhite = false;
			}
			else if (Board::column(from) == 0)
			{
				boardParsed()->m_castleAvailableQueenWhite = false;
			}
		}
		else if (typeid(*fromPiece) == typeid(Pawn))
		{
			// En passant
			if (!Board::sameColumn(from, to) && m_board->board()[to] == nullptr)
			{
				// Should never be nullptr
				UInt enPassantTile = to + (fromPiece->isWhite() ? -BOARD_SIZE : BOARD_SIZE);
				delete m_board->board()[enPassantTile];
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
		}

		if (toPiece != nullptr)
		{
			delete m_board->board()[to];
		}

		m_board->board()[from] = nullptr;

		m_board->board()[to] = fromPiece;
		fromPiece->tile() = to;

		// Editing color table
		if (m_isWhiteTurn)
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
		m_board->enPassant(typeid(*fromPiece) == typeid(Pawn) && fabs(Int(from) - Int(to)) == 16 ? Board::column(to) : -1);
	}

	/**
		* @brief Fills the	board with the position fen
		*
		* @param fen representation to fill the board
		* @return true if board was successfully filled.
		* @return false else
		*/
	bool fillBoard(std::string fen)
	{

		std::stringstream sstream(fen);
		std::string word;
		std::vector<std::string> words{};
		while (std::getline(sstream, word, ' '))
		{
			words.push_back(word);
		}

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
						m_board->board()[counter] = NULL;
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
				m_board->board()[counter] = NULL;
			}

			switch (c)
			{
			case 'p':
				m_board->board()[counter] = new Pawn(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'P':
				m_board->board()[counter] = new Pawn(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'k':
				m_board->board()[counter] = new King(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'K':
				m_board->board()[counter] = new King(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'q':
				m_board->board()[counter] = new Queen(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'Q':
				m_board->board()[counter] = new Queen(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'r':
				m_board->board()[counter] = new Rook(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'R':
				m_board->board()[counter] = new Rook(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'b':
				m_board->board()[counter] = new Bishop(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'B':
				m_board->board()[counter] = new Bishop(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'n':
				m_board->board()[counter] = new Knight(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'N':
				m_board->board()[counter] = new Knight(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case '/':
				counter -= BOARD_SIZE * 2 + 1;
				break;
			default:
				m_board->board()[counter] = nullptr;
				break;
			}
			++counter;
		}

		if (words.size() > 1 && words[1] == "w")
		{
			m_isWhiteTurn = true;
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
				}
				if (std::find(words[2].begin(), words[2].end(), 'q') != words[2].end())
				{
					m_board->m_castleAvailableQueenBlack = true;
				}
				if (std::find(words[2].begin(), words[2].end(), 'K') != words[2].end())
				{
					m_board->m_castleAvailableKingWhite = true;
				}
				if (std::find(words[2].begin(), words[2].end(), 'k') != words[2].end())
				{
					m_board->m_castleAvailableKingBlack = true;
				}
			}
		}
		if (words.size() > 4 && words[3] != "-")
		{
			m_board->enPassant(std::stoi(words[3]) - 1);
		}
		return true;
	}

	bool inCheck(bool isWhite) const
	{
		UInt kingPos = 0;
		std::vector<UInt> vTotal = std::vector<UInt>();
		// finding king
		for (const UInt tile : (isWhite ? m_board->whitePos() : m_board->blackPos()))
		{
			const Piece *piece = m_board->board()[tile];
			if (piece != nullptr && typeid(*piece) == typeid(King))
			{
				kingPos = tile;
				break;
			}
		}
		// computing all oponents moves
		for (const UInt tile : (!isWhite ? m_board->whitePos() : m_board->blackPos()))
		{
			std::vector<UInt> v;
			const Piece *piece = m_board->board()[tile];
			if (piece == nullptr)
			{
				continue;
			}
			piece->canMove(*m_board, v);
			vTotal.insert(vTotal.end(), v.begin(), v.end());
		}

		// TODO: parallelize
		return std::find(vTotal.begin(), vTotal.end(), kingPos) != vTotal.end();
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
		std::cout << out << std::string("|") << std::endl;
	}

	/**
		* @brief Display board in CLI and shows how can a piece move.
		*
		*/
	void displayCLIMove(UInt tile)
	{
		std::string out;
		const Piece *piece = m_board->board()[tile];
		std::vector<UInt> v = std::vector<UInt>();
		if (piece != nullptr)
		{
			piece->canMove(*m_board, v);
		}

		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			const Piece *value = m_board->board()[counter];
			out.append("|");
			if (std::find(v.begin(), v.end(), counter) != v.end())
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
		if (std::find(v.begin(), v.end(), BOARD_SIZE - 1) != v.end())
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

		std::cout << out << std::string("|") << std::endl;
	}
};
