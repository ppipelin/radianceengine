#include "include.h"
#include "board.h"
#include "uci.h"

#include <iostream>

Bitboard Bitboards::bbPieces[PieceType::NB] = {};
Bitboard Bitboards::bbColors[Color::COLOR_NB] = {};

int main(int argc, char *argv[])
{
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
