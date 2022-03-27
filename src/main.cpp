#include "include.h"
#include "board.h"
#include "boardParser.h"
#include "connect.h"

#include "annex.h"

#include <iostream>
#include <omp.h>
#include <ctime>

int main()
{
	// BoardParser bParser = BoardParser();
	// bParser.fillBoard("rnb1kbnr/pppp1ppp/8/4p3/7q/5P1P/PPPPP1P1/RNBQKBNR");
	// bParser.fillBoard("rnbqkbnr/pppppppp/8/5N2/8/5B2/PPPPPPPP/RNBQK2R");
	// bParser.fillBoard("r3k2r/pppppppp/8/1bnq1N1n/4b3/5B2/PPPPPPPP/RNBQK2R");
	// bParser.displayCLI();
	// std::cout << std::endl;
	// bParser.displayCout();
	// std::cout << std::endl;
	// for (UInt i = 0; i < BOARD_SIZE2; ++i)
	// {
	// 	bParser.displayCLIMove(i);
	// 	std::cout << std::endl;
	// }
	// bParser.displayCLIMove(4);
	// std::cout << std::endl;
	// bParser.displayCLIWhiteBlack();

	// benchFillBoard();
	// benchInCheck();
	// benchInCheck(/*display =*/ false, 1000); // 1000 release with vector - 960 with reserve on canmove - 910 with array
	// benchInCheck(/*display =*/ false, 10000); // 12 225 release with vector - 9023 with reserve on canmove -  8429 with array
	benchRules(3, false);
	// Depth 0: 1
	// Depth 1: 20 - 0ms ---- Size in Mo 0.033144 -> 0.013432 
	// Depth 2: 400 - 3ms -> 1ms ---- Size in Mo 0.033144 -> 0.028024
	// Depth 3: 8902 - 64ms -> 46ms ---- Size in Mo 0.662424 ->0.560024
	// Depth 4: 197 742 - 1136ms -> 925ms ---- Size in Mo 14.7417 -> 12.4631
	// Depth 5: 4 896 998 - 25249ms -> 21350ms ---- Size in Mo 327.461 -> 276.857
	// perft(5) done in 15s with empty ram
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

	// connect();

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
