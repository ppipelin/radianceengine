#pragma once

#include "include/include.h"
#include "include/board.h"

// Abstract Piece class
class Piece
{
protected:
 Int m_tile;
 bool m_isWhite;
 bool m_neverMoved;
public:
 Piece( bool isWhite, bool neverMoved, Int a, Int b ) : Piece( isWhite, neverMoved, 64 - (BOARD_SIZE*b + (BOARD_SIZE-a)) ) {}
 Piece(bool isWhite, bool neverMoved, Int tile) : m_isWhite( isWhite ), m_neverMoved( neverMoved ), m_tile( tile ) {} 
 Piece(){}
 ~Piece(){}
 // Returns a vector of the cases where Piece can move.
 // Can also use a pair to split free moves and takings // virtual std::pair< std::vector<Int>, std::vector<Int> > canMove( Board &b ) {
 virtual std::vector<Int> canMove( Board &b ) {
  return std::vector<Int>();
 }

 const bool isWhite() const {
  return m_isWhite;
 }

 virtual bool exists() {
  return false;
 }

 virtual const std::string str() {
  return " ";
 }
};