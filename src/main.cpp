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
	// bParser.displayCLI();
	// std::cout << std::endl;
	// bParser.displayCout();
	// std::cout << std::endl;
	// bParser.displayCLIMove();
	// std::cout << std::endl;
	// bParser.displayCLIWhiteBlack();

	// benchFillBoard();
	// benchInCheck();
	// benchInCheck(/*display =*/ false, 1000); // 1000 release with vector - 960 with reserve on canmove - 910 with array
	// benchInCheck(/*display =*/ false, 10000); // 12 225 release with vector - 9023 with reserve on canmove -  8429 with array
	benchRules(4);
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
