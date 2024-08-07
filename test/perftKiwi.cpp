#include "../include/search.h"

Bitboard Bitboards::bbPieces[PieceType::NB] = {};
Bitboard Bitboards::bbColors[Color::COLOR_NB] = {};

int main(int argc, char *argv[])
{
	BoardParser b;
	BoardParser::State s;
	const std::string kiwiFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

	b.fillBoard(kiwiFen, &s);

	return (Search::perft(b, 2, false) != 2039) || (Search::perft(b, 3, false) != 97862) || (Search::perft(b, 4, false) != 4085603);
}
