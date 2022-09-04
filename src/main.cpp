#include "include.h"
#include "board.h"
#include "boardParser.h"
// #include "connect.h"
#include "client.h"
#include "cMove.h"

#include "annex.h"

#include <iostream>
#include <chrono>

int main()
{
	Game game = Game();
	Client client = Client(game);
	client.connect();

	// Classic Perft Results
	// 4 -       197 281
	// 5 -     4 865 609 with en passant + castle
	// 6 -   119 060 324 - 1 207 806ms
	game.m_boardParser->fillBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");

	// Kiwipete position
	// 1 -	           48 -- Result: 48
	// 2	-         2 039         2 039
	// 3	-        97 862        97 862
	// 4 -     4 085 603
	// 5 -   193 690 690
	// 6 - 8 031 647 685
	game.m_boardParser->fillBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"); // Pos 2 - 3 passed
	// game.m_boardParser->fillBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"); // Pos 3 - 6 passed
	// game.m_boardParser->fillBoard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"); // Pos 4 - 3 passed
	// Position 5
	// 1 -           	44 -- Result: 44
	// 2 - 	       1 486 -- Result: 1 487
	// 3 - 	      62 379
	// 4 -    	2 103 487
	// 5	-    89 941 194
	// game.m_boardParser->fillBoard("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"); // Pos 5 - 1 passed
	// game.m_boardParser->fillBoard("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"); // Pos 6 - 4 passed (5 is 164 075 514 instead of 164 075 551)

	// game.m_boardParser->displayCLI();
	// std::cout << std::endl;
	// game.m_boardParser->displayCout();
	// std::cout << std::endl;
	// for (UInt i = 0; i < BOARD_SIZE2; ++i)
	// {
	// 	game.m_boardParser->displayCLIMove(i);
	// 	std::cout << std::endl;
	// }
	// game.m_boardParser->displayCLIWhiteBlack();

	// int perft_value = 4;
	// auto t1 = std::chrono::high_resolution_clock::now();
	// std::cout << "perft(" << perft_value << ") : " << perft(game.m_boardParser-> perft_value, true) << std::endl;
	// auto t2 = std::chrono::high_resolution_clock::now();
	// auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	// std::cout << "Total time for benchInCheck: " << ms_int.count() << "ms" << std::endl;

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
