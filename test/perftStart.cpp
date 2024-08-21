#include <string>

#include "board.h"
#include "boardParser.h"
#include "include.h"
#include "search.h"

Bitboard Bitboards::bbPieces[PieceType::NB] = {};
Bitboard Bitboards::bbColors[Color::COLOR_NB] = {};

Bitboard Bitboards::movesRook[BOARD_SIZE2] = {};
Bitboard Bitboards::movesRookMask[BOARD_SIZE2] = {};
std::unordered_map<Bitboard, Bitboard> Bitboards::movesRookLegal[BOARD_SIZE2] = {};

int main(int, char **)
{
	bbInit();
	BoardParser b;
	BoardParser::State s;
	const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	b.fillBoard(startFen, &s);

	return (Search::perft(b, 2, false) != 400) || (Search::perft(b, 3, false) != 8902) || (Search::perft(b, 4, false) != 197281) || (Search::perft(b, 5, false) != 4865609);
}
