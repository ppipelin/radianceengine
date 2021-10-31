#pragma once

#include "include.h"

#include "piece.h"
#include "pawn.h"
#include "king.h"
#include "queen.h"

#include "board.h"

class BoardParser
{
private:
	Board m_board;
	const std::string m_starting = std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	const std::string m_pawnMoveTest = std::string("rnbqkbnr/pppppppp/1P6/8/8/8/PPPPPPPP/RNBQKBNR");

public:
	BoardParser(Board &board) : m_board(board)
	{
		// fillBoard(m_starting);
		fillBoard(m_pawnMoveTest);
	}

	~BoardParser() {}

	// True if board was successfully filled.
	bool fillBoard(std::string fen)
	{

		// #pragma omp parallel for // Impossible because counter changes because of fen digit system
		// for ( std::int16_t counter = 0; counter < BOARD_SIZE*BOARD_SIZE; ++counter )

		// till 63
		for (Int counter = 0; const auto & c : fen)
		{
			if (isdigit(c))
			{
				for (std::int16_t i = 0; i < atoi(&c); ++i)
				{
					if (m_board.board()[counter] != nullptr)
						delete m_board.board()[counter];
					m_board.board()[counter] = new Piece(false, false, counter);
					// m_board.empty()[counter] = true;
					++counter;
				}
				continue;
			}
			if (c == '/' && (counter + 1) % BOARD_SIZE == 0)
			{
				std::cout << "ERR: going further than board numbers." << std::endl;
				return false;
			}
			switch (c)
			{
			case 'p':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new Pawn(false, true, counter);
				break;
			case 'P':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new Pawn(true, true, counter);
				break;
			case 'k':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new King(false, true, counter);
				break;
			case 'K':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new King(true, true, counter);
				break;
			case 'q':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new Queen(false, true, counter);
				break;
			case 'Q':
				if (m_board.board()[counter] != nullptr)
					delete m_board.board()[counter];
				m_board.board()[counter] = new Queen(true, true, counter);
				break;
			case '/':
				--counter;
				break;
			default:
				//  m_board.empty()[counter] = true;
				// std::cout << "default: " << c << std::endl;
				break;
			}
			++counter;
		}
		return true;
	}

	void displayCLI()
	{
		std::string out;
		for (Int counter = 0; const auto & value : m_board.board())
		{
			if (counter % BOARD_SIZE == 0 && counter != 0)
				out.append("|\n");
			out.append("|");
			std::string s;
			if (value == nullptr)
			{
				out.append(" ");
			}
			else
			{
				s = value->str();
				out.append(s);
			}
			++counter;
		}
		std::cout << out << std::string("|") << std::endl;
	}

	void displayCLIMove()
	{
		std::string out;
		std::vector<Int> bypass = std::vector<Int>();
		for (Int counter = 0; const auto & value : m_board.board())
		{
			if (counter % BOARD_SIZE == 0 && counter != 0)
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

			if (value == nullptr)
			{
				out.append(" ");
			}
			else
			{
				std::string s = value->str();
				out.append(s);
				std::vector<Int> moves = value->canMove(m_board);
				if (!moves.empty() && bypass.empty())
				{
					bypass = std::vector<Int>(moves);
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
		for (const auto &value : m_board.board())
		{
			if (value == nullptr)
			{
				std::cout << " ";
				continue;
			}
			std::cout << value->str();
		}
	}
};
