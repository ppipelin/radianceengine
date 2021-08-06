#pragma once

#include "include/include.h"
;
class Board
{
 private:
  const std::string m_starting = std::string( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
  std::vector<Int> m_boardOld;
  std::vector<Piece*> m_board;
 public:
  Board() {
    // m_boardOld = std::vector<Int>( BOARD_SIZE * BOARD_SIZE);
    // fillBoardOld(m_starting);
    m_board.reserve( BOARD_SIZE * BOARD_SIZE);
    for (Int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
    {
     m_board.push_back(new Piece(i));
    }
    fillBoard(m_starting);
  }

  void displayCLIOld() {
    std::string out;
    Int counter = 0;
    for (auto value : m_boardOld)
    {
      if ( keyPieceOld.find(value) != keyPieceOld.end() )
        continue;
      if (counter % BOARD_SIZE == 0 && counter != 0)
        out.append("|\n");
      out.append("|");
      std::string s;
      s = pieceKeyOld.at(value);
      out.append(s);
      ++counter;
    }
    std::cout << out << std::string("|") << std::endl;
  }

  void displayCLI() {
    std::string out;
    Int counter = 0;
    for (auto value : m_board)
    {
      if (counter % BOARD_SIZE == 0 && counter != 0)
        out.append("|\n");
      out.append("|");
      std::string s;
      s = value->str();
      out.append(s);
      ++counter;
    }
    std::cout << out << std::string("|") << std::endl;
  }

  void displayCoutOld() {
    for (auto value : m_boardOld)
    {
      std::cout << value << std::endl;
    }
  }
  
  void displayCout() {
    for (auto value : m_board)
    {
      std::cout << value->str() << std::endl;
    }
  }

  // True if board was successfully filled.
  bool fillBoardOld( std::string fen ) {
    Int counter = 0; // till 63
    // #pragma omp for
    for ( auto c : fen )
    {
      if ( isdigit(c) )
      {
        for ( Int i = 0; i < atoi(&c); ++i)
        {
          m_boardOld[counter] =  keyPieceOld.at(' ');
          ++counter;
        }
        continue;
      }
      if ( c == '/' && (counter + 1) % BOARD_SIZE == 0 )
      {
        std::cout << "ERR: going to far " << std::endl;
        return false;
      }
      if ( keyPieceOld.find(c) != keyPieceOld.end() )
      {
        m_boardOld[counter] = keyPieceOld.at(c);
        ++counter;
      }
    }
    return true;
  }

  // True if board was successfully filled.
  bool fillBoard( std::string fen ) {
   Int counter = 0; // till 63
   // #pragma omp for
   for ( auto c : fen )
   {
    if ( isdigit(c) )
    {
     for ( Int i = 0; i < atoi(&c); ++i)
     {
      m_board[counter] = new Piece(counter);
      ++counter;
     }
     continue;
    }
    if ( c == '/' && (counter + 1) % BOARD_SIZE == 0 )
    {
     std::cout << "ERR: going further than board numbers." << std::endl;
     return false;
    }
    switch (c)
    {
     case 'p':
      delete m_board[counter];
      m_board[counter] = new Pawn(false, counter);
      break;
     case 'P':
      delete m_board[counter];
      m_board[counter] = new Pawn(true, counter);
      break;
    case 'k':
      delete m_board[counter];
      m_board[counter] = new King(false, counter);
      break;
    case 'K':
      delete m_board[counter];
      m_board[counter] = new King(true, counter);
      break;
    case 'q':
      delete m_board[counter];
      m_board[counter] = new Queen(false, counter);
      break;
    case 'Q':
      delete m_board[counter];
      m_board[counter] = new Queen(true, counter);
      break;
     case '/':
      --counter;
      break;
     default:
      // std::cout << "default: " << c << std::endl;
      break;
    }
    ++counter;
   }
   return true;
  }

  bool isFree(Int tile) {
    return m_board[tile]->exists();
  }
};
