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
	// Kiwipete position
	// 1 -	           48 -- Result: 48
	// 2	-         2 039          2 043
	// 3	-        97 862         97 763
	// 4 -     4 085 603
	// 5 -   193 690 690
	// 6 - 8 031 647 685
	boardParser.fillBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");

	// Position 5
	// 1 -        	44 -- Result: 41
	// 2 - 	    1 486
	// 3 - 	   62 379
	// 4 - 	2 103 487
	// 5	- 89 941 194

	boardParser.fillBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

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
	auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "Total time for benchInCheck: " << ms_int.count() << "ms" << std::endl;

	// Classic Perft Results
	// perft(4) : 197 281 - 1 976ms
	// perft(5) : 4 865 351 
	// perft(5) : 4 865 609 with en passant + castle
	// perft(5) : 4 865 609 - 47 796ms with correct delete
	// perft(6) : 119 060 324 - 1 207 806ms


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
