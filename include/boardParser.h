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
		fillBoard(m_starting);
		// fillBoard(m_pawnMoveTest);
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
	}

	BoardParser(const BoardParser &b)
	{
		m_board = new Board();
		for (UInt i = 0; i < BOARD_SIZE2; ++i)
		{
			Piece *p = b.board()->board()[i];
			if (p == nullptr)
				continue;
			if (typeid(*p) == typeid(Piece))
				m_board->board()[i] = new Piece((*b.board())[i]);
			else if (typeid(*p) == typeid(King))
				m_board->board()[i] = new King((*b.board())[i]);
			else if (typeid(*p) == typeid(Queen))
				m_board->board()[i] = new Queen((*b.board())[i]);
			else if (typeid(*p) == typeid(Rook))
				m_board->board()[i] = new Rook((*b.board())[i]);
			else if (typeid(*p) == typeid(Bishop))
				m_board->board()[i] = new Bishop((*b.board())[i]);
			else if (typeid(*p) == typeid(Knight))
				m_board->board()[i] = new Knight((*b.board())[i]);
			else if (typeid(*p) == typeid(Pawn))
				m_board->board()[i] = new Pawn((*b.board())[i]);
		}
	}

	// Mutators
	Board *board() { return m_board; }

	const Board *board() const { return m_board; }

	void movePiece(UInt	from, UInt to)
	{
		Piece *fromPiece = m_board->board()[from];
		Piece *toPiece = m_board->board()[to];
		if (fromPiece == nullptr)
		{
			return;
		}
		if (toPiece != nullptr)
		{
			delete m_board->board()[to];
		}

		m_board->board()[from] = NULL;

		m_board->board()[to] = fromPiece;
		fromPiece->tile() = to;
		// TODO : move color tables, not sure if really efficient, see erase(remove())
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

			if (value == nullptr)
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
			if (value == nullptr)
			{
				std::cout << " ";
				continue;
			}
			std::cout << value->str();
		}
	}
};
