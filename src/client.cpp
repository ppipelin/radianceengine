#include "client.h"
#include "cMove.h"
#include "utils.h"

Client::Client(Game &game)
{
	m_game = &game;
	m_cya = false;
	m_connect_finish = false;
}

Client::~Client()
{}

void Client::drop(sio::event &e)
{
	// std::cout << "from: " << e.get_messages()[0]->get_string() << std::endl;
	// std::cout << "to: " << e.get_messages()[1]->get_string() << std::endl;
	std::string fromString = e.get_messages()[0]->get_string();
	std::string toString = e.get_messages()[1]->get_string();
	if (fromString == toString)
		return;
	UInt from = Board::toTiles(fromString);
	UInt to = Board::toTiles(toString);
	cMove move = cMove(from, to);

	const Piece *piece = (*m_game->m_boardParser->boardParsed())[from];

	if (piece == nullptr || piece->isWhite() != m_game->m_boardParser->isWhiteTurn() || !piece->canMove(*(m_game->m_boardParser->boardParsed()), move))
	{
		// Have to send the cancel message to the server.
		m_currentSocket->emit("cpp_unmakeLastMove");
		return;
	}
	BoardParser boardParser = BoardParser(*m_game->m_boardParser);
	boardParser.movePiece(move);
	if (boardParser.inCheck(piece->isWhite()))
	{
		// In check, have to send the cancel message to the server.
		m_currentSocket->emit("cpp_unmakeLastMove");
		return;
	}

	m_game->m_boardParser->movePiece(move);
	// std::cout << m_boardParser->boardParsed()->board()[Board::toTiles(fromString)]->str() << " from " << m_boardParser->boardParsed()->board()[Board::toTiles(e.get_messages()[0]->get_string())]->tile() << std::endl;
	// std::cout << m_boardParser->boardParsed()->board()[Board::toTiles(toString)]->str() << " to " << m_boardParser->boardParsed()->board()[Board::toTiles(e.get_messages()[1]->get_string())]->tile() << std::endl;
	// Once we received an accepted drop we compute and execute next move
	m_currentSocket->emit("cpp_move", utils::to_string(m_game->makeNextMove()));
}

void Client::dragStart(sio::event &e)
{
	std::cout << "dragStart: " << e.get_message()->get_string() << std::endl;
	std::vector<cMove> v;
	Piece *piece = (*m_game->m_boardParser->boardParsed())[Board::toTiles(e.get_message()->get_string())];
	piece->canMove(*m_game->m_boardParser->boardParsed(), v);
	std::cout << "sending: " << utils::to_string(v) << std::endl;
	m_currentSocket->emit("cpp_moveset", utils::to_string(v));
}

void Client::triggerExit(sio::event &)
{
	m_cya = true;
}

void Client::bindEvents()
{
	m_currentSocket->on("dropToCPP", [&](sio::event &e) {return drop(e);});
	m_currentSocket->on("dragStartToCPP", [&](sio::event &e) {return dragStart(e);});
	m_currentSocket->on("triggerExit", [&](sio::event &e) {return triggerExit(e);});
}

int Client::connect()
{
	ConnectionListener listener(m_lock, m_cond, m_connect_finish);
	m_handler.set_open_listener(std::bind(&ConnectionListener::on_connected, &listener));
	m_handler.set_close_listener(std::bind(&ConnectionListener::on_close, &listener, std::placeholders::_1));
	m_handler.set_fail_listener(std::bind(&ConnectionListener::on_fail, &listener));
	m_handler.connect("http://localhost:3000");
	m_lock.lock();
	if (!m_connect_finish)
	{
		m_cond.wait(m_lock);
	}
	m_lock.unlock();
	m_currentSocket = m_handler.socket();

	m_lock.lock();
	if (false)
	{
		m_cond.wait(m_lock);
	}
	m_lock.unlock();

	bindEvents();

	while (!m_cya)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10s);
	}

	m_handler.sync_close();
	m_handler.clear_con_listeners();

	return 0;
}
