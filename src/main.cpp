#include "include.h"
#include "board.h"
#include "boardParser.h"
// #include "connect.h"
#include "client.h"

#include "annex.h"

#include <iostream>
#include <chrono>

int main()
{
	BoardParser boardParser = BoardParser();
	Client	client = Client(boardParser);
	client.connect();

	// boardParser.fillBoard("rnb1kbnr/pppp1ppp/8/4p3/7q/5P1P/PPPPP1P1/RNBQKBNR");
	// boardParser.fillBoard("rnbqkbnr/pppppppp/8/5N2/8/5B2/PPPPPPPP/RNBQK2R");
	// boardParser.fillBoard("r3k2r/pppppppp/8/1bnq1N1n/4b3/5B2/PPPPPPPP/RNBQK2R");
	// boardParser.displayCLI();
	// std::cout << std::endl;
	// boardParser.displayCout();
	// std::cout << std::endl;
	// for (UInt i = 0; i < BOARD_SIZE2; ++i)
	// {
	// 	boardParser.displayCLIMove(i);
	// 	std::cout << std::endl;
	// }
	// boardParser.displayCLIWhiteBlack();

	// int perft_value = 3;
	// auto t1 = std::chrono::high_resolution_clock::now();
	// std::cout << "perft(" << perft_value << ") : " << perft(boardParser, perft_value) << std::endl;
	// auto t2 = std::chrono::high_resolution_clock::now();
	// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	// std::cout << "Total time for benchInCheck: " << ms_int.count() << "ms" << std::endl;
	// perft(4) : 197 281
	// perft(5) : 4 865 351
	// perft(5) : 4 865 609 with en passant + castle

	// RES should be : 
	// 1
	// 20
	// 400
	// 8 902
	// 197 281
	// 4 865 609
	// 119 060 324
	// 3 195 901 860

	// demo();

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
