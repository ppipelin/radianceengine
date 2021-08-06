#pragma once

#include "piece.h"

class Pawn : public Piece 
{
 private:
  bool m_isWhite;

 public:
  // a is letter, b is number. Both starts at 0
  // tile is computed by substracting to 64 the sum of the cases from the bottom right to left.
  Pawn( bool isWhite, Int a, Int b ) : Piece( 64 - (BOARD_SIZE*b + (BOARD_SIZE-a)) ), m_isWhite( isWhite ) {}
  Pawn( bool isWhite, Int a) : Piece( a ), m_isWhite( isWhite ) {}
  ~Pawn(){}
  // Returns a vector of the cases where Piece can move.
  std::vector<Int> canMove()
  {
    std::vector<Int> v;
    if(m_isWhite)
    {
      if( floor( m_tile / BOARD_SIZE ) > 0 )
      {
        v.push_back( m_tile - BOARD_SIZE );
        if( floor( m_tile / BOARD_SIZE ) == BOARD_SIZE-2 )
          v.push_back( m_tile - BOARD_SIZE*2 );
      }
    }
    else
    {
      if( floor( m_tile / BOARD_SIZE ) < BOARD_SIZE-1 )
      {
        v.push_back( m_tile + BOARD_SIZE );
        if( floor( m_tile / BOARD_SIZE ) == 1 )
          v.push_back( m_tile + BOARD_SIZE*2 );
      }
    }
    return v;
  }

  bool exists() override {
   return true;
  }

  const std::string str() override {
   return m_isWhite ? "P" : "p";
  }
};