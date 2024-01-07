#include "uci.h"

#include "cMove.h"
#include "boardParser.h"
#include "search.h"
#include "searchRandom.h"
#include "searchMaterialist.h"
#include "searchMaterialistNegamax.h"
#include "evaluate.h"
#include "evaluateShannon.h"

#include <queue>
#include <chrono>

namespace {
	const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	void position(BoardParser &pos, std::istringstream &is)
	{
		cMove c;
		std::string token, fen;

		is >> token;
		if (token == "startpos")
		{
			fen = startFen;
			is >> token; // Consume the "moves" token, if any
		}
		else if (token == "fen")
		{
			while (is >> token && token != "moves")
				fen += token + " ";
		}
		else
			return;

		pos.fillBoard(fen);

		// Parse the moves list, if any
		while (is >> token && (c = UCI::to_move(pos, token)) != cMove())
		{
			pos.movePiece(c);
		}
	}

	// setoption() is called when the engine receives the "setoption" UCI command.
	// The function updates the UCI option ("name") to the given value ("value").
	void setoption(std::istringstream &is)
	{
		std::string token, name, value;

		is >> token; // Consume the "name" token

		// Read the option name (can contain spaces)
		while (is >> token && token != "value")
			name += (name.empty() ? "" : " ") + token;

		// Read the option value (can contain spaces)
		while (is >> token)
			value += (value.empty() ? "" : " ") + token;

		if (Options.count(name))
			Options[name] = value;
		else
			std::cout << "No such option: " << name << std::endl;
	}

	// go() is called when the engine receives the "go" UCI command. The function
	// sets the thinking time and other parameters from the input string, then starts
	// with a search.
	void go(const BoardParser &pos, std::istringstream &is)
	{
		Search::LimitsType limits;
		std::string token;

		// limits.startTime = now(); // The search starts as early as possible

		while (is >> token)
		{
			if (token == "searchmoves") // Needs to be the last command on the line
				while (is >> token)
					// limits.searchmoves.push_back(UCI::to_move(pos, token));
					;
			// else if (token == "wtime")     is >> limits.time[WHITE];
			// else if (token == "btime")     is >> limits.time[BLACK];
			// else if (token == "winc")      is >> limits.inc[WHITE];
			// else if (token == "binc")      is >> limits.inc[BLACK];
			// else if (token == "movestogo") is >> limits.movestogo;
			else if (token == "depth")     is >> limits.depth;
			// else if (token == "nodes")     is >> limits.nodes;
			// else if (token == "movetime")  is >> limits.movetime;
			// else if (token == "mate")      is >> limits.mate;
			else if (token == "perft")     is >> limits.perft;
			// else if (token == "infinite")  limits.infinite = 1;
			// else if (token == "ponder")    ponderMode = true;
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		if (limits.perft > 0)
		{
			std::cout << "Nodes searched: " << Search::perft(pos, limits.perft) << std::endl;
		}
		else
		{
			// SearchRandom search = SearchRandom(limits);
			// SearchMaterialist search = SearchMaterialist(limits);
			SearchMaterialistNegamax search = SearchMaterialistNegamax(limits);
			EvaluateShannon evaluate = EvaluateShannon();

			cMove move = search.nextMove(pos, evaluate);
			std::cout << "bestmove " << UCI::move(move) << std::endl;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "Total time for go: " << ms_int.count() << "ms" << std::endl;
	}
}

/// UCI::loop() waits for a command from the stdin, parses it and then calls the appropriate
/// function. It also intercepts an end-of-file (EOF) indication from the stdin to ensure a
/// graceful exit if the GUI dies unexpectedly. When called with some command-line arguments,
/// like running 'bench', the function returns immediately after the command is executed.
/// In addition to the UCI ones, some additional debug commands are also supported.
void UCI::loop(int argc, char *argv[])
{
	BoardParser pos;
	std::string token, cmd;

	pos.fillBoard(startFen);

	std::queue<std::string> q;
	for (int i = 1; i < argc; ++i)
		q.push(std::string(argv[i]));
	{
		// q.push("position startpos moves e2e3");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3");
		// q.push("go");
		// q.push("position startpos moves d7d5 d1h5 d8d7 g1f3 c7c5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5");
		// q.push("go");
		// q.push(" position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3 h8g8");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3 h8g8 d3f4");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3 h8g8 d3f4 f8d6");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3 h8g8 d3f4 f8d6 f4d5");
		// q.push("go");
		// q.push("position startpos moves e2e3 d7d5 d1h5 d8d7 g1f3 c7c5 f3e5 g7g6 e5d7 g6h5 d7c5 e8d8 f1b5 e7e5 c5d3 c8f5 d3e5 g8h6 b5d3 f5d3 e5d3 h8g8 d3f4 f8d6 f4d5 g8g2");
		// q.push("go");
	}
	do
	{
		do // do-while until queue is empty
		{
			if (!q.empty())
			{
				cmd = q.front();
				std::cout << "queue (" << q.size() << ") command: " << cmd << std::endl;
				q.pop();
			}
			else
			{
				if (argc == 1 && !getline(std::cin, cmd)) // Wait for an input or an end-of-file (EOF) indication
					cmd = "quit";
			}

			std::istringstream is(cmd);

			token.clear(); // Avoid a stale if getline() returns nothing or a blank line
			is >> std::skipws >> token;

			if (token == "quit" || token == "stop")
				std::cout << "UCI - quitting..." << std::endl;

			// The GUI sends 'ponderhit' to tell that the user has played the expected move.
			// So, 'ponderhit' is sent if pondering was done on the same move that the user
			// has played. The search should continue, but should also switch from pondering
			// to the normal search.
			else if (token == "d")
				pos.displayCLI();
			else if (token == "ponderhit")
				std::cout << "UCI - ponderhit received" << std::endl;
			else if (token == "uci")
				std::cout << "id name chessengine" << "\n" << Options << "\nuciok" << std::endl;
			else if (token == "setoption")
				setoption(is);
			else if (token == "go")
				go(pos, is);
			else if (token == "position")
				position(pos, is);
			else if (token == "isready")
				std::cout << "readyok" << std::endl;
			else if (token == "eval")
				std::cout << "UCI - eval called" << std::endl;
			else if (token == "--help" || token == "help" || token == "--license" || token == "license")
				std::cout << "\nRadiance is chess engine for playing and analyzing."
				"\nIt supports Universal Chess Interface (UCI) protocol to communicate with a GUI, an API, etc."
				"\nor read the corresponding README.md and Copying.txt files distributed along with this program." << std::endl;
			else if (!token.empty() && token[0] != '#')
				std::cout << "UCI - Unknown command: '" << cmd << "'." << std::endl;
		} while (!q.empty());
	} while (token != "quit" && argc == 1); // The command-line arguments are one-shot
}

/// UCI::square() converts a const UInt to a string in algebraic notation (g1, a7, etc.)
std::string UCI::square(const UInt s)
{
	return Board::toString(s);
}

/// UCI::move() converts a Move to a string in coordinate notation (g1f3, a7a8q).
std::string UCI::move(cMove m)
{
	if (m == MOVE_NONE)
		return "(none)";

	if (m == MOVE_NULL)
		return "0000";

	const UInt from = m.getFrom();
	const UInt to = m.getTo();

	std::string move = Board::toString(from) + Board::toString(to);

	if (m.isPromotion())
		move += "nbrq"[m.getFlags() & 0x3]; // keep last two bits

	return move;
}

/// UCI::to_move() converts a string representing a move in coordinate notation
/// (g1f3, a7a8q) to the corresponding legal Move, if any.
cMove UCI::to_move(const BoardParser &pos, std::string &str)
{
	UInt flags = 0;
	UInt from = Board::toTiles(str.substr(0, 2));
	UInt to = Board::toTiles(str.substr(2, 2));
	// Capture flag
	if ((*pos.boardParsed())[to] != nullptr)
		flags |= 0x4;
	if (str.length() == 5)
	{
		// The promotion piece character must be lowercased
		str[4] = char(tolower(str[4]));
		flags |= 0x8;
		switch (str[4])
		{
		case 'b':
			flags |= 0x1;
			break;
		case 'r':
			flags |= 0x2;
			break;
		case 'q':
			flags |= 0x3;
			break;
		}
	}

	const Piece *p = (*pos.boardParsed())[from];
	// Detect castle and flag
	if (p != nullptr && typeid(*p) == typeid(King))
	{
		if (to - from == 2)
		{
			flags = 0x2;
		}
		else if (from - to == 2)
		{
			flags = 0x3;
		}
	}

	return cMove(from, to, flags);
}

// UCI::pv() formats PV information according to the UCI protocol. UCI requires
// that all (if any) unsearched PV lines are sent using a previous search score.
std::string UCI::pv(const Search &s, const BoardParser &b, UInt depth)
{
	std::stringstream ss;
	const std::array<Search::RootMove, MAX_PLY> &rootMoves = s.rootMoves;

	for (UInt i = 0; i < 1; ++i) // s.rootMovesSize
	{
		// Not at first line
		if (ss.rdbuf()->in_avail())
			ss << "\n";

		ss << "info"
			<< " depth " << depth
			<< " nodes " << s.nodesSearched[i]
			<< " multipv " << i
			<< " score cp " << rootMoves[i].score
			<< " pv";

		auto a = std::count_if(rootMoves[i].pv.begin(), rootMoves[i].pv.end(), [](const cMove c) { return c != 0; });
		for (UInt j = 0; j < a; ++j)
			ss << " " << UCI::move(rootMoves[i].pv[j]);
	}
	return ss.str();
}
