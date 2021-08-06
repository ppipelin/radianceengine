#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <regex>
#include <map>

// typedef short Int;
typedef std::uint_fast16_t Int;

const Int BOARD_SIZE = 8; // final is possible ?
const std::map< char, Int > keyPieceOld = std::map< char, Int >
{
 { 'k', 0 },
 { 'q', 1 },
 { 'r', 2 },
 { 'b', 3 },
 { 'n', 4 },
 { 'p', 5 },
 { ' ', 6 },
 { 'K', 7 },
 { 'Q', 8 },
 { 'R', 9 },
 { 'B', 10 },
 { 'N', 11 },
 { 'P', 12 }
};

const std::map< Int, char > pieceKeyOld = std::map< Int, char >
{
 { 0, 'k' },
 { 1, 'q' },
 { 2, 'r' },
 { 3, 'b' },
 { 4, 'n' },
 { 5, 'p' },
 { 6, ' ' },
 { 7, 'K' },
 { 8, 'Q' },
 { 9, 'R' },
 { 10, 'B' },
 { 11, 'N' },
 { 12, 'P' },
};

#include "include/piece.h"
#include "include/pawn.h"
#include "include/king.h"
#include "include/queen.h"
