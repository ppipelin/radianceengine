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

class BoardParser
{
private:
	Board *m_board;
	const std::string m_starting = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	const std::string m_pawnMoveTest = std::string("rnbqkbnr/pppppppp/1P6/8/8/8/PPPPPPPP/RNBQKBNR");

public:
	BoardParser()
	{
		m_board = new Board();
		// fillBoard(m_starting);
		fillBoard(m_pawnMoveTest);
	}

	~BoardParser()
	{
		for (auto &i : m_board->board())
		{
			delete i;
		}
	}
	// Mutators
	Board *board() { return m_board; }

	const Board *board() const { return m_board; }

	/**
		* @brief Fills the	board with the position fen
		*
		* @param fen representation to fill the board
		* @return true if board was successfully filled.
		* @return false else
		*/
	bool fillBoard(std::string fen)
	{

		// #pragma omp parallel for // Impossible because counter changes because of fen digit system
		// for ( std::int16_t counter = 0; counter < BOARD_SIZE2; ++counter )

		// till 63
		m_board->whitePos().clear();
		m_board->blackPos().clear();
		m_board->whitePos().reserve(BOARD_SIZE * 2);
		m_board->blackPos().reserve(BOARD_SIZE * 2);
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; const auto & c : fen)
		{
			if (isdigit(c))
			{
				for (Int i = 0; i < atoi(&c); ++i)
				{
					m_board->board()[counter] = new Piece(counter);
					++counter;
				}
				continue;
			}
			if (c == '/' && Board::column(counter + 1) == 0)
			{
				err("Going further than board numbers.");
				return false;
			}
			switch (c)
			{
			case 'p':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Pawn(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'P':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Pawn(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'k':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new King(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'K':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new King(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'q':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Queen(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'Q':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Queen(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'r':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Rook(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'R':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Rook(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'b':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Bishop(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'B':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Bishop(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case 'n':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Knight(counter, false, true);
				m_board->blackPos().push_back(counter);
				break;
			case 'N':
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Knight(counter, true, true);
				m_board->whitePos().push_back(counter);
				break;
			case '/':
				counter -= BOARD_SIZE * 2 + 1;
				break;
			default:
				if (m_board->board()[counter] != nullptr)
					delete m_board->board()[counter];
				m_board->board()[counter] = new Piece(counter, true, true);
				break;
			}
			++counter;
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
			if (typeid(*m_board->board()[tile]) == typeid(King))
			{
				kingPos = tile;
				break;
			}
		}
		// computing all oponents moves
		for (const UInt tile : (!isWhite ? m_board->whitePos() : m_board->blackPos()))
		{
			std::vector<UInt> v;
			m_board->board()[tile]->canMove(*m_board, v);
			vTotal.insert(vTotal.end(), v.begin(), v.end());
		}

		// TODO: parallelize
		for (UInt i = 0; i < vTotal.size(); ++i)
		{
			if (vTotal[i] == kingPos)
				return true;
		}
		return false;
	}

	void displayCLI()
	{
		std::string out;
		for (UInt counter = BOARD_SIZE2 - BOARD_SIZE; counter != BOARD_SIZE - 1; ++counter)
		{
			const Piece *value = m_board->board()[counter];
			out.append("|");
			if (!value->exists())
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
		if (!value->exists())
		{
			out.append(" ");
		}
		else
		{
			out.append(value->str());
		}

		std::cout << out << std::string("|") << std::endl;
	}

	/**
		* @brief Display board in CLI by parsing the optimized vectors.
		*
		*/
	void displayCLIWhiteBlack()
	{
		std::string out;
		for (UInt counter = 0; counter < BOARD_SIZE2; ++counter)
		{
			if (Board::column(counter) == 0 && counter != 0)
				out.append("|\n");
			out.append("|");
			std::string s;
			auto whiteFind = std::find(m_board->whitePos().begin(), m_board->whitePos().end(), counter);
			auto blackFind = std::find(m_board->blackPos().begin(), m_board->blackPos().end(), counter);
			if (whiteFind != m_board->whitePos().end())
			{
				out.append("X");
			}
			else if (blackFind != m_board->blackPos().end())
			{
				out.append("O");
			}
			else
			{
				out.append(" ");
			}
		}
		std::cout << out << std::string("|") << std::endl;
	}

	void displayCLIMove()
	{
		std::string out;
		std::vector<UInt> bypass = std::vector<UInt>();
		for (UInt counter = 0; const auto & value : m_board->board())
		{
			if (Board::column(counter) == 0 && counter != 0)
				out.append("|\n");
			out.append("|");

			if (!bypass.empty())
			{
				bool doesContinue = false;
				for (const auto &b : bypass)
				{
					if (b == counter)
					{
						out.append("X");
						++counter;
						doesContinue = true;
						break;
					}
				}
				if (doesContinue)
					continue;
			}

			if (!value->exists())
			{
				out.append(" ");
			}
			else
			{
				std::string s = value->str();
				out.append(s);
				std::vector<UInt> moves;
				value->canMove(*m_board, moves);
				if (!moves.empty() && bypass.empty())
				{
					bypass = std::vector<UInt>(moves);
				}
			}
			++counter;
		}
		std::cout << out << std::string("|") << std::endl;
		// if ( bypass != nullptr ) delete bypass;
	}

	void displayCout()
	{
		std::cout << "displayCout" << std::endl;
		for (const Piece *value : m_board->board())
		{
			if (!value->exists())
			{
				std::cout << " ";
				continue;
			}
			std::cout << value->str();
		}
	}
};
