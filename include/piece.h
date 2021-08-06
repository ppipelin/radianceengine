#pragma once

#include "include/include.h"

// Abstract Piece class
class Piece
{
 protected:
  Int m_tile;
 public:
  Piece(Int tile) : m_tile( tile ) {} 
  Piece(){}
  ~Piece(){}
  // Returns a vector of the cases where Piece can move.
  std::vector< Int > canMove() {
   return std::vector< Int >();
  }

  virtual bool exists() {
   return false;
  }

  virtual const std::string str() {
   return " ";
  }
};