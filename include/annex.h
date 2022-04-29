#pragma once

#include "include.h"
#include "boardParser.h"

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

std::vector<std::string> fens = { "rnb1kbnr/pppp1ppp/8/4p3/7q/5P1P/PPPPP1P1/RNBQKBNR", "rnb2bnr/ppqp1k2/2p4R/6N1/5p2/1P6/PBPPQPP1/RN2KB2", "r5k1/2q2ppp/1p1Qp3/2n1n3/r3P3/P1P1BP2/3RNKPP/7R", "5bk1/5p2/7p/4Q1pn/3P2K1/4NPPP/2N2q2/8", "2r1r1k1/1bqnbppp/ppp5/5P2/2NNQ3/4P3/PP3PBP/2RR2K1", "r3r1k1/pppbqp1p/3b2p1/4n1B1/2N1n3/2Q2NP1/PP2PPBP/R4RK1", "5rk1/1br5/pp1p2qp/3Pnp1n/PP5B/N6P/2BQ2PK/2R2R2", "r3r1k1/pp1q1ppp/2ppb3/8/1bP5/4P1P1/PBQP1PBP/R4RK1", "r1b1r2k/ppq3b1/2p2npp/P7/1P2N2Q/2P3PP/1B3P1K/R3RB2", "5r2/pp5k/7p/5p2/3RN1r1/2P5/bP4PP/5RK1", "3rn1k1/6pp/1b3p2/4p3/2q5/R1P3P1/P1QB1P1P/2N3K1", "5r1r/6p1/p1bN2kp/1pR1Pp2/1n6/1P4P1/4B3/3RK3", "3r2k1/7p/3Pq1p1/pQ3p2/P3p3/3n2PP/1P3PB1/3R2K1", "1r3rk1/p1pqRpp1/2p4p/2P4Q/5Nb1/6P1/PBP2P1P/1R4K1", "1n2k2r/1p2bpp1/p1r1p1p1/P2pP3/QP1q4/6P1/3BBP1P/R1R3K1", "6k1/3q1pp1/1p2p2p/p2n1n2/3P4/1P1Q1BPP/P4PK1/B7", "2k4r/pbpr2pp/1p3n2/2b5/2P1q3/2Q1BN1P/PP2BPP1/R3K2R", "r4rk1/pp4pp/2n1b2q/3ppB2/3P2Q1/2N1P3/PP4PP/4RRK1", "r3r1k1/1p3p1p/1pp2qp1/4n3/1P1pP1P1/P2P1nBP/4QPB1/R3R2K", "2r2rk1/p2qnppp/1pn1p3/3pP3/Q4P2/P2P1N2/1P3BPP/R4RK1", "r3r1k1/1p3ppp/4b3/pP2B2N/P7/4qQ2/5bPP/R2R3K", "4r1k1/1b3p1p/pn4p1/1p1Nq3/1P1rP3/6P1/1Q2RPBP/4R1K1", "r2n2k1/1r3p1p/pp1q1np1/8/N1pP4/3QPN1P/P1R2PP1/2R3K1", "5b2/7k/3qp2p/p2B4/P4r2/7P/1Q3P2/6RK", "4k1r1/1p1b1p1p/pq2pB2/8/3R3P/5N2/5PP1/2R1K3", "8/p4ppk/6qp/5Q2/2P1n3/4NnPP/P4P2/5B1K", "r1b2rk1/pp3ppp/3qp3/8/6N1/2P1R3/P3QPPP/R5K1", "2q1r1k1/p3ppbp/1rn3p1/8/R1bP4/2N1P1PP/5PB1/1QBR2K1", "r1q1kb1r/pQ3p1p/np1p1p2/1N2p3/2P5/8/PP2PPPP/R3KB1R", "2r1r3/5kpp/3p1n2/p3nP1P/1p1B4/6NK/PPP2R2/2R5", "5rk1/2pqb1pp/R1n1p1b1/1p6/1P6/1BPpBN1P/5PP1/3Q2K1", "6k1/2q1pp1p/3p2p1/3P1b2/PnQ1N3/3rP1P1/5PBP/2R3K1", "1r3rk1/q2nbppp/b1n1p3/p1ppP3/5PP1/2PP1NNP/P1QB2B1/4RRK1", "2r4k/1pp4p/7b/p1pPp2P/2P1N3/5r2/PPK1QPq1/R4R2", "4r1k1/pp3ppp/1b6/3q1r2/1P1B4/P2Q1NP1/3P2KP/R6R", "r4bk1/1p1r1ppb/1qp1pn1p/p2p4/2PP4/PPQ1P1P1/1B1N1PBP/2RR2K1", "2r1k2r/pp1n1ppp/3bbn2/q2p2B1/3Pp3/1BN2N2/PPP2PPP/R3QRK1", "3r2k1/p3p1bp/1q2P3/5p2/3Nb3/4B1Q1/P2R1KPP/8", "2r2rk1/1b3pbp/pq1pp1p1/8/3NPPPP/2NRQ3/PPP5/1K4R1", "r5k1/5pb1/2q1bQ1p/pp4pB/1P2p3/P3P1BP/5PPK/3R4", "2rr2k1/2b2pp1/2P1pn1p/8/4Pq2/3B4/5QPP/2RR2BK", "5rk1/1p1R2bp/p5pq/6r1/2P1pnQ1/1PN4P/P4PP1/B4RK1", "r5k1/ppq2pp1/2p5/3prN1Q/8/2P2PnP/PP4P1/R3R1K1", "k2r4/pp6/1np5/5N1p/3P4/4PQ2/PP1q4/1K1R4", "r7/1p1npkbp/p3p1p1/4n3/2rBN3/6PB/qPP1Q2P/2KR3R", "4q1k1/5p2/3p1Bp1/1b1P4/4PQ2/2p5/rpP3P1/1R4K1", "5r1k/p1pq2p1/1p1br2p/3QppP1/2R4P/2PP4/P1P2PK1/2B1R3", "1r6/5pkp/r2p2p1/2qQ4/2p1PP2/3n2PP/P1R3BK/3R4", "r5k1/1pprqbb1/3p2p1/pNn4p/2P1p3/1P2P1BP/P1Q1BPP1/2RR2K1", "r1bq1rk1/b1pp2pp/p1n5/1p1Np1B1/2Q5/5NP1/PPP2nBP/R4RK1", "1r4k1/2pbqp1p/3n1R2/2pPp3/p3P3/4N1P1/Pr4BP/2Q2RK1", "3r1rk1/pb2np2/1p1qp1pp/4R3/3P4/P1NQ2P1/BP3PP1/4R1K1", "rn2nrk1/1pp1qpp1/p6p/3Pp3/2B1P1bN/2P1B3/P1Q2PPP/R4RK1", "r1r3k1/5ppp/4pn2/p1q5/2Q5/P1B1P3/5PPP/1R1R2K1", "4rk2/5pp1/p3b2r/1p1p1B2/6Pp/5QqP/PPP5/1K2RR2", "r2r4/pp3pkp/3p2p1/3Bn3/q3b3/PNQ3P1/5P1P/1R1R2K1", "1k1r2r1/pp3p1p/1b5q/3P1p2/N3p1b1/P1Q3P1/1PR1NP1P/3R2K1", "2r1r3/p2k2pp/2q1p3/1pbp1p2/2nP1P2/P1R2NPP/1PQ2P2/5RK1", "5r1k/pp2q1pb/2p2n1p/3p4/3P2PP/2P1rBQ1/PP1N2K1/4RR2", "r4rk1/pp1bbppp/1n2p3/2n1P3/8/2N1BN2/PP2BPPP/R2R2K1", "r2q1rk1/pp1nbppp/2p1pn2/5b2/2QP4/2N2NP1/PP2PPBP/R1BR2K1", "r2r4/1p3pkp/4pnp1/p1P1n3/1P5q/P6P/2BRNPP1/3QR1K1", "r4rk1/1p1bbppp/p7/3pqN2/8/1P1QB2P/1PP2RP1/R5K1", "r2r2k1/2q2pp1/1n1bp2p/p3N3/P1pPQ3/2P3P1/3N1PP1/1R2R1K1", "r1q1r1k1/pp1n1pb1/2pB2pp/4p1n1/2P1P1Q1/1P4PB/P3NP1P/3RR1K1", "r1rn2k1/2p3bp/p2qbp2/2R1p3/NQ1nP1pN/6B1/PP3PPP/R4B1K", "4r3/R3pqbk/5p2/1p2pN1p/2p1P2P/2P2P2/1P2Q3/7K", "4R3/1p3pk1/p2q2p1/3P2p1/1P4P1/P1r4P/5PK1/3Q4", "3r1k2/3q2pp/3r4/3npp2/2Bb4/5QB1/1P3PPP/R2R2K1", "r4rk1/1bq1bppp/p4n2/2p1p3/N2PP3/BP1B4/P3QPPP/R3R1K1", "r4k2/5p2/2Np2pp/2pP4/3bP3/6P1/1rb2PBP/2R1R1K1", "3r2k1/2p2p2/pq3bpp/1p1P4/1PP5/4r3/P2N1QPP/2RR2K1", "6k1/2n1ppb1/1p4pp/2p5/q1PP3P/2B1PNP1/2b1QP2/5BK1", "r2r2k1/1b2qpp1/p7/1p1n3Q/1b1B4/1P2N2P/P4PP1/2R2RK1", "4r1k1/p4p2/2R3pQ/3q1r2/1B2p1n1/P3P1P1/1P5P/2R3K1", "4rr1k/ppp3qp/3nP1p1/8/Q2bp2P/B5P1/PP3PB1/2R2RK1", "8/pb1r1pk1/2P1pq1p/2R3pP/4B3/P1Q3P1/3r1P2/6K1", "r4rk1/p4ppp/1p1p4/3QpnN1/6q1/P1R3P1/1P2PP1P/5RK1", "2k1b3/r1p1b3/pr2q1pp/N2RP3/1P1Q1P2/P1R5/2P3P1/2K5", "6k1/pp3ppp/2n1r3/8/2pP1B2/2P2qP1/P2Q1P1P/4R1K1", "2b2qk1/1p5p/2n3p1/5p2/2rp1QN1/2n2B2/1R3PPP/4R1K1", "r4n2/np3pk1/p2p2pp/2pPr3/q1P1P2B/1R6/1PQ1BP1P/6RK", "2r1r1k1/1b1q1p1p/pQn2bpB/3B4/8/P4N2/5PPP/3RR1K1", "4r1k1/p3b3/1pq4p/3Rp1p1/2P1N3/1P3QP1/P6P/6K1", "1r1n2r1/2qkbpNp/pp2p1nP/3pP3/P1pP4/2P1B1PQ/2B3K1/R4R2", "r4r2/1R3p1p/2kqp3/p1n5/1B1p1P2/2Q5/2P3PP/1R4K1", "4r1k1/5p1p/pqn1p1p1/N2pb3/1P6/P5P1/5P1P/1R1Q1BK1", "4r1k1/2p2pp1/2r1n1p1/1p6/1P2R1Pb/2PP4/2BB1P2/R5K1", "2rq1rk1/p2pbppp/1pb1pn2/n7/1PP5/3Q1NP1/P2NPPBP/1RB2RK1", "6r1/1ppq3k/n2p4/p1nPp2B/2P1Pr1P/2N1Q1R1/PP6/2R3K1", "4rbk1/5p1p/q5pP/2p1P3/1n1pQ3/5NP1/4RPK1/3R4", "2r3k1/r1q4p/p3pBp1/1p1bP3/4p1Q1/2P5/PP2BbP1/1K1R3R", "4r2k/7p/p5q1/N1P5/Q2P2p1/7P/5RP1/6K1", "4rbk1/2qr1pp1/7p/ppnB1Q2/7P/2B3P1/PP1R1P2/3R2K1" };
void benchFillBoard()
{
	BoardParser bParser = BoardParser();
	auto t1 = high_resolution_clock::now();
	int i = 0;
	for (std::string fen : fens)
	{
		++i;
		bParser.fillBoard(fen);
	}
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);
	std::cout << "Total time for benchFillBoard: " << ms_int.count() << "ms" << std::endl;
}

void benchInCheck(bool display = false, UInt multiplier = 1)
{
	std::vector<BoardParser *>	bParsers;
	bParsers.reserve(fens.size());
	for (std::string fen : fens)
	{
		BoardParser *bParser = new BoardParser();
		bParser->fillBoard(fen);
		bParsers.push_back(bParser);
	}
	auto t1 = high_resolution_clock::now();
	for (UInt i = 0; i < multiplier; ++i)
	{
		for (UInt counter = 0; const auto & bParser : bParsers)
		{
			if (bParser->inCheck(true))
			{
				if (display)
				{
					std::cout << "In check white " << counter << std::endl;
					bParser->displayCLI();
				}
			}
			if (bParser->inCheck(false))
			{
				if (display)
				{
					std::cout << "In check black " << counter << std::endl;
					bParser->displayCLI();
				}
			}
			++counter;
		}
	}
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);
	std::cout << "Total time for benchInCheck: " << ms_int.count() << "ms" << std::endl;
}

/**
	* @brief perft test from https://www.chessprogramming.org/Perft
	*
	* @param b
	* @param depth
	* @return UInt number of possibles position after all possible moves on b
	*/
UInt perft(BoardParser &b, UInt depth = 1)
{
	std::vector<std::pair<UInt, UInt> > moveList = std::vector<std::pair<UInt, UInt> >();
	UInt nodes = 0;
	for (UInt tile = 0; tile < BOARD_SIZE2; ++tile)
	{
		// #define TEST_CHECK_COVERING
		std::vector<UInt> subMoveList = std::vector<UInt>();
		const Piece *piece = b.boardParsed()->board()[tile];
		if (piece == nullptr || (piece->isWhite() != b.isWhiteTurn()))
		{
			continue;
		}
		if (piece->isWhite() != b.isWhiteTurn())
		{
			warn("is wrong turn");
		}
		piece->canMove(*b.boardParsed(), subMoveList);
		std::vector<std::pair<UInt, UInt> > subMoveListPairs = std::vector<std::pair<UInt, UInt> >();
		subMoveListPairs.reserve(subMoveList.size());
		for (UInt to : subMoveList)
		{
			subMoveListPairs.push_back(std::make_pair(tile, to));
		}

		moveList.insert(moveList.end(), subMoveListPairs.begin(), subMoveListPairs.end());
	}
	if (depth == 0)
	{
		return 1;
	}
#ifdef TEST_CHECK_COVERING
	bool wasInCheck = false;

	if (b.inCheck(b.isWhiteTurn()))
	{
		wasInCheck = true;
	}
#endif
	for (std::pair<UInt, UInt> move : moveList)
	{
		BoardParser b2 = BoardParser(b);
		b2.movePiece(move.first, move.second);
		// We assert that we are not in check after we just moved
		if (b2.inCheck(!b2.isWhiteTurn()))
		{
			// We don't count illegal moves
			continue;
		}
#ifdef TEST_CHECK_COVERING
		if (wasInCheck)
		{
			debug("Check covered : ");
			b2.displayCLI();
		}
#endif
		nodes += perft(b2, depth - 1);
	}
	return nodes;
}

void fill(std::vector<UInt> &tofill)
{
	for (int i = 0; i < 4; ++i)
	{
		tofill.push_back(i);
	}
}

void fill2(std::vector<Piece> &tofill)
{
	for (int i = 0; i < 4; ++i)
	{
		tofill.push_back(King(i, true, true));
	}
}

void fill3(std::vector<Piece *> &tofill)
{
	for (int i = 0; i < 4; ++i)
	{
		tofill.push_back(new King(i, true, true));
	}
}

int demo()
{
	std::vector<UInt> a;
	a.reserve(4);
	fill(a);
	for (auto tile : a)
	{
		std::cout << tile << std::endl;
	}

	std::vector<Piece> aa;
	aa.reserve(4);
	fill2(aa);
	for (auto tile : aa)
	{
		std::cout << tile.str() << std::endl;
	}

	std::vector<Piece *> aaa;
	aaa.reserve(4);
	fill3(aaa);
	for (auto tile : aaa)
	{
		std::cout << tile->str() << std::endl;
	}
	return 0;
}
