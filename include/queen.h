#pragma once

#include "piece.h"

class Queen : public Piece 
{
 private:
  bool m_isWhite;

 public:
  // a is letter, b is number. Both starts at 0
  // tile is computed by substracting to 64 the sum of the cases from the bottom right to left.
  Queen( bool isWhite, Int a, Int b ) : Piece( 64 - (BOARD_SIZE*b + (BOARD_SIZE-a)) ), m_isWhite( isWhite ) {}
  Queen( bool isWhite, Int a) : Piece( a ), m_isWhite( isWhite ) {}
  ~Queen() {}
  // Returns a vector of the cases where Piece can move.
  std::vector<Int> canMove()
  {
    std::vector<Int> v;
    if(m_isWhite)
    {
    }
    else
    {
    }
    return v;
  }

  bool exists() override {
   return true;
  }

  const std::string str() override {
   return m_isWhite ? "Q" : "q";
  }
};