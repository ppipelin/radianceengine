#include "include.h"
#include "board.h"
#include "boardParser.h"
#include "connect.h"

#include <iostream>
#include <omp.h>
#include <ctime>

int main()
{
	Board b = Board();
	BoardParser bParser(b);
	// bParser.displayCLI();
	// bParser.displayCout();
	std::cout << std::endl;
	bParser.displayCLIMove();

	std::cout << "Hello, world!" << std::endl;
	connect();
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
