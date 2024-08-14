#include "search.h"

Bitboard Bitboards::bbPieces[PieceType::NB] = {};
Bitboard Bitboards::bbColors[Color::COLOR_NB] = {};

int main(int, char **)
{
	BoardParser b;
	BoardParser::State s;
	const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	b.fillBoard(startFen, &s);

	return (Search::perft(b, 2, false) != 400) || (Search::perft(b, 3, false) != 8902) || (Search::perft(b, 4, false) != 197281) || (Search::perft(b, 5, false) != 4865609);
}
