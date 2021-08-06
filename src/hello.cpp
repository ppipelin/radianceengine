#pragma once

#include <iostream>
#include "include/include.h"
#include "include/board.h"
// #include "include/piece.h"
// #include "include/pawn.h"

int main()
{
 // displays Hello World! on screen
 // std::cout<<"Hello World!\n";
 Board b = Board();
 // b.displayCLIOld();
 // b.displayCoutOld();
 b.displayCLI();
 // b.displayCout();
 return 0;
}
