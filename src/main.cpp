#include <iostream>
#include <unordered_map>

#include "board.h"
#include "boardParser.h"
#include "include.h"
#include "uci.h"

Bitboard Bitboards::bbPieces[PieceType::NB] = {};
Bitboard Bitboards::bbColors[Color::COLOR_NB] = {};

Bitboard Bitboards::movesRook[BOARD_SIZE2] = {};
Bitboard Bitboards::movesRookMask[BOARD_SIZE2] = {};
std::unordered_map<Bitboard, Bitboard> Bitboards::movesRookLegal[BOARD_SIZE2] = {};

int main(int argc, char *argv[])
{
	bbInit();
	UCI::init(g_options);
	UCI::loop(argc, argv);
	std::cout << "Hello, world!" << std::endl;
	return 0;
}

// int omp()
// {
//  int i;
//  int threadID = 0;
//  #pragma omp parallel for private(i, threadID)
//  for(i = 0; i < 100; i++ )
//  {
//   threadID = omp_get_thread_num();
//   #pragma omp critical
//   {
//    printf("Thread %d reporting\n", threadID);
//   }
//  }
//  return 0;
// }
