#include <iostream>
#include <omp.h>
#include <ctime>

#include "include/include.h"
#include "include/board.h"
#include "include/boardParser.h"
// #include "include/piece.h"
// #include "include/pawn.h"

int main()
{
 // displays Hello World! on screen
 // std::cout<<"Hello World!\n";
 Board b = Board();
 BoardParser bParser( b );
 // bParser.displayCLI();
 // bParser.displayCout();
 std::cout << std::endl;
 bParser.displayCLIMove();
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